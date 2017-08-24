/*************************************************************************//**
 
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
#include "config.h"
#include "stdio.h" //printl
//#include "protect.h"
#include "string.h"
#include "fs.h"
#include"assert.h"
#include "proc.h"
//#include "tty.h"
#include "console.h"

#include "hd.h"

PRIVATE void init_fs();
PRIVATE void mkfs();

//5M~6M,self define,low level memory management
u8* fsbuf=(u8*)FSBUF;//0x500000;
//const int FSBUF_SIZE=0x100000;
/**
 * Since all invocations of `rw_sector()' in FS look similar (most of the
 * params are the same), we use this macro to make code more readable.
 *
 * Before I wrote this macro, I found almost every rw_sector invocation
 * line matchs this emacs-style regex:
 * `rw_sector(\([-a-zA-Z0-9_>\ \*()+.]+,\)\{3\}\ *SECTOR_SIZE,\ *TASK_FS,\ *fsbuf)'
 */
 //奇怪的事：给rw_sector再添加个参数fsbuf,传进去会变成别的值，猜测是哪越界，将rw_sector的参数区域修改了。
#define RD_SECT(dev,sect_nr) rw_sector(DEV_READ, \
				       dev,				\
				       (sect_nr) * SECTOR_SIZE,		\
				       SECTOR_SIZE, /* read one sector */ \
				       TASK_FS);
#define WR_SECT(dev,sect_nr) do{ \
				rw_sector(DEV_WRITE,  dev, \
				       (sect_nr) * SECTOR_SIZE, \
				       SECTOR_SIZE, /* write one sector */ \
				       TASK_FS); \
				}while(0);
				
/*****************************************************************************
 *                                task_fs
 *****************************************************************************/
/**
 * <Ring 1> The main loop of TASK FS.
 * 
 *****************************************************************************/
PUBLIC void task_fs()
{
	printl("Task FS begins.\n");
	init_fs();
	spin("FS");
}
/* FS related below */
/* For dd_map[k],
 * `k' is the device nr.\ dd_map[k].driver_nr is the driver nr.
 *
 * Remeber to modify include/hd.h if the order is changed. *****************************************************************************/
struct dev_drv_map dd_map[] = {
	/* driver nr.		major device nr.
	   ----------		---------------- */
	{INVALID_DRIVER},	/**< 0 : Unused */
	{INVALID_DRIVER},	/**< 1 : Reserved for floppy driver */
	{INVALID_DRIVER},	/**< 2 : Reserved for cdrom driver */
	{TASK_HD},		/**< 3 : Hard disk */
	{TASK_TTY},		/**< 4 : TTY */
	{INVALID_DRIVER}	/**< 5 : Reserved for scsi disk driver */
};

/*****************************************************************************
 *                                init_fs
 *****************************************************************************/
/**
 * <Ring 1> Do some preparation.
 * 
 *****************************************************************************/
PRIVATE void init_fs()
{
	/* open the device: hard disk */
	MESSAGE driver_msg;
	driver_msg.type = DEV_OPEN;
	driver_msg.DEVICE = MINOR(ROOT_DEV);
	assert(dd_map[MAJOR(ROOT_DEV)].driver_nr != INVALID_DRIVER);
	send_recv(BOTH, dd_map[MAJOR(ROOT_DEV)].driver_nr, &driver_msg);
//printl("start mkfs\n");
	mkfs();
}

/*****************************************************************************
 *                                mkfs
 *****************************************************************************/
/**
 * <Ring 1> Make a available Orange'S FS in the disk. It will
 *          - Write a super block to sector 1.
 *          - Create three special files: dev_tty0, dev_tty1, dev_tty2
 *          - Create the inode map
 *          - Create the sector map
 *          - Create the inodes of the files
 *          - Create `/', the root directory
 *****************************************************************************/
PRIVATE void mkfs()
{
	MESSAGE driver_msg;
	int i, j;

	int bits_per_sect = SECTOR_SIZE * 8; /* 8 bits per byte */

	/* get the geometry of ROOTDEV */
	struct part_info geo;
	driver_msg.type		= DEV_IOCTL;
	driver_msg.DEVICE	= MINOR(ROOT_DEV);
	driver_msg.REQUEST	= DIOCTL_GET_GEO;
	driver_msg.BUF		= &geo;
	driver_msg.PROC_NR	= TASK_FS;
	assert(dd_map[MAJOR(ROOT_DEV)].driver_nr != INVALID_DRIVER);
	send_recv(BOTH, dd_map[MAJOR(ROOT_DEV)].driver_nr, &driver_msg);

	printl("dev size: 0x%x sectors\n", geo.size);

	/************************/
	/*      super block     */
	/************************/
	struct super_block sb;
	sb.magic	  = MAGIC_V1;
	sb.nr_inodes	  = bits_per_sect;
	sb.nr_inode_sects = sb.nr_inodes * INODE_SIZE / SECTOR_SIZE;
	sb.nr_sects	  = geo.size; /* partition size in sector */
	sb.nr_imap_sects  = 1;
	sb.nr_smap_sects  = sb.nr_sects / bits_per_sect + 1;
	sb.n_1st_sect	  = 1 + 1 +   /* boot sector & super block */
		sb.nr_imap_sects + sb.nr_smap_sects + sb.nr_inode_sects;
	sb.root_inode	  = ROOT_INODE;
	sb.inode_size	  = INODE_SIZE;
	struct inode x;
	sb.inode_isize_off= (int)&x.i_size - (int)&x;
	sb.inode_start_off= (int)&x.i_start_sect - (int)&x;
	sb.dir_ent_size	  = DIR_ENTRY_SIZE;
	struct dir_entry de;
	sb.dir_ent_inode_off = (int)&de.inode_nr - (int)&de;
	sb.dir_ent_fname_off = (int)&de.name - (int)&de;

	memset(fsbuf, 0x90, SECTOR_SIZE);
	memcpy(fsbuf, &sb, SUPER_BLOCK_SIZE);

	/* write the super block */
	WR_SECT(ROOT_DEV, 1);
/*printl("there\n");
	printl("devbase:0x%x00, sb:0x%x00, imap:0x%x00, smap:0x%x00\n"
	       "        inodes:0x%x00, 1st_sector:0x%x00\n", 
	       geo.base * 2,
	       (geo.base + 1) * 2,
	       (geo.base + 1 + 1) * 2,
	       (geo.base + 1 + 1 + sb.nr_imap_sects) * 2,
	       (geo.base + 1 + 1 + sb.nr_imap_sects + sb.nr_smap_sects) * 2,
	       (geo.base + sb.n_1st_sect) * 2);
*/
	/************************/
	/*       inode map      */
	/************************/
	memset(fsbuf, 0, SECTOR_SIZE);
	for (i = 0; i < (NR_CONSOLES + 2); i++)
		fsbuf[0] |= 1 << i;

	assert(fsbuf[0] == 0x1F);/* 0001 1111 : 
				  *    | ||||
				  *    | |||`--- bit 0 : reserved
				  *    | ||`---- bit 1 : the first inode,
				  *    | ||              which indicates `/'
				  *    | |`----- bit 2 : /dev_tty0
				  *    | `------ bit 3 : /dev_tty1
				  *    `-------- bit 4 : /dev_tty2
				  */
	WR_SECT(ROOT_DEV, 2);

	/************************/
	/*      secter map      */
	/************************/
	memset(fsbuf, 0, SECTOR_SIZE);
	int nr_sects = NR_DEFAULT_FILE_SECTS + 1;
	/*             ~~~~~~~~~~~~~~~~~~~|~   |
	 *                                |    `--- bit 0 is reserved
	 *                                `-------- for `/'
	 */
	for (i = 0; i < nr_sects / 8; i++)
		fsbuf[i] = 0xFF;

	for (j = 0; j < nr_sects % 8; j++)
		fsbuf[i] |= (1 << j);

	WR_SECT(ROOT_DEV, 2 + sb.nr_imap_sects);

	/* zeromemory the rest sector-map */
	memset(fsbuf, 0, SECTOR_SIZE);
	for (i = 1; i < sb.nr_smap_sects; i++)
		WR_SECT(ROOT_DEV, 2 + sb.nr_imap_sects + i);

	/************************/
	/*       inodes         */
	/************************/
	/* inode of `/' */
	memset(fsbuf, 0, SECTOR_SIZE);
	struct inode * pi = (struct inode*)fsbuf;
	pi->i_mode = I_DIRECTORY;
	pi->i_size = DIR_ENTRY_SIZE * 4; /* 4 files:
					  * `.',
					  * `dev_tty0', `dev_tty1', `dev_tty2',
					  */
	pi->i_start_sect = sb.n_1st_sect;
	pi->i_nr_sects = NR_DEFAULT_FILE_SECTS;
	/* inode of `/dev_tty0~2' */
	for (i = 0; i < NR_CONSOLES; i++) {
		pi = (struct inode*)(fsbuf + (INODE_SIZE * (i + 1)));
		pi->i_mode = I_CHAR_SPECIAL;
		pi->i_size = 0;
		pi->i_start_sect = MAKE_DEV(DEV_CHAR_TTY, i);//改为设备号
		pi->i_nr_sects = 0;
	}
	WR_SECT(ROOT_DEV, 2 + sb.nr_imap_sects + sb.nr_smap_sects);

	/************************/
	/*          `/'         */
	/************************/
	memset(fsbuf, 0, SECTOR_SIZE);
	struct dir_entry * pde = (struct dir_entry *)fsbuf;

	pde->inode_nr = 1;
	strcpy(pde->name, ".");

	/* dir entries of `/dev_tty0~2' */
	for (i = 0; i < NR_CONSOLES; i++) {
		pde++;
		pde->inode_nr = i + 2; /* dev_tty0's inode_nr is 2 */
		sprintf(pde->name, "dev_tty%d", i);
	}
	WR_SECT(ROOT_DEV, sb.n_1st_sect);
}

/*****************************************************************************
 *                                rw_sector
 *****************************************************************************/
/**
 * <Ring 1> R/W a sector via messaging with the corresponding driver.
 * 
 * @param io_type  DEV_READ or DEV_WRITE
 * @param dev      device nr
 * @param pos      Byte offset from/to where to r/w.
 * @param bytes    r/w count in bytes.
 * @param proc_nr  To whom the buffer belongs.
 * @param buf      r/w buffer.
 * 
 * @return Zero if success.
 *****************************************************************************/
PUBLIC int rw_sector(int io_type, int dev, u64 pos, int bytes, int proc_nr)//,
		    // void* fsbuf)
{
	MESSAGE driver_msg;
printl("*rw_sector--BUF:%x\n",fsbuf);
	driver_msg.type		= io_type;
	driver_msg.DEVICE	= MINOR(dev);
	driver_msg.POSITION	= pos;
	driver_msg.BUF		= fsbuf; //--my-------------------------
	driver_msg.CNT		= bytes;
	driver_msg.PROC_NR	= proc_nr;
	assert(dd_map[MAJOR(dev)].driver_nr != INVALID_DRIVER);

	send_recv(BOTH, dd_map[MAJOR(dev)].driver_nr, &driver_msg);
//printl("rw_sector end\n");
	return 0;
}

