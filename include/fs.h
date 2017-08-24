/*************************************************************************//**
 *****************************************************************************
 * @file   include/sys/fs.h
 * @brief  Header file for File System.
 * @author Forrest Yu
 * @date   2008
 *****************************************************************************
 *****************************************************************************/

#ifndef	_FS_H_
#define _FS_H_

#include"type.h"

/**
 * @struct dev_drv_map fs.h "include/sys/fs.h"
 * @brief  The Device_nr.\ - Driver_nr.\ MAP.
 *
 *  \dot
 *  digraph DD_MAP {
 *    graph[rankdir=LR];
 *    node [shape=record, fontname=Helvetica];
 *    b [ label="Device Nr."];
 *    c [ label="Driver (the task)"];
 *    b -> c [ label="DD_MAP", fontcolor=blue, URL="\ref DD_MAP", arrowhead="open", style="dashed" ];
 *  }
 *  \enddot
 */
struct dev_drv_map {
	int driver_nr; /**< The proc nr.\ of the device driver. */
};

/**
 * @def   MAGIC_V1
 * @brief Magic number of FS v1.0
 */
#define	MAGIC_V1	0x111

/**
 * @struct super_block fs.h "include/fs.h"
 * @brief  The 2nd sector of the FS
 *
 * Remember to change SUPER_BLOCK_SIZE if the members are changed.
 *
 * @attention Remember to change boot/include/load.inc::SB_* if the members
 *            are changed.
 *
 * \dot
 * digraph super_block {
 *         node [shape=plaintext];
 * 
 *         sb [label=<<TABLE CELLPADDING="0" CELLSPACING="0" BORDER="0">
 *                 <TR>
 *                         <TD HEIGHT="20" WIDTH="70" ALIGN="right" VALIGN="top" BORDER="0">Sector #&nbsp;</TD>
 *                         <TD HEIGHT="20" WIDTH="90" ALIGN="left" VALIGN="top" BORDER="0">0</TD>
 *                         <TD HEIGHT="20" WIDTH="90" ALIGN="left" VALIGN="top" BORDER="0">1</TD>
 *                         <TD HEIGHT="20" WIDTH="290" ALIGN="center" VALIGN="top" BORDER="0">...</TD>
 *                         <TD HEIGHT="20" WIDTH="10" ALIGN="right" BORDER="0"></TD>
 *                 </TR>
 *                 <TR>
 *                         <TD HEIGHT="30" WIDTH="70" ALIGN="right" BORDER="0"></TD>
 *                         <TD HEIGHT="30" WIDTH="90" ALIGN="center" BORDER="1" BGCOLOR="white">Boot Sector</TD>
 *                         <TD HEIGHT="30" WIDTH="90" ALIGN="center" BORDER="1" BGCOLOR="grey">Super Block</TD>
 *                         <TD HEIGHT="30" WIDTH="290" ALIGN="center" BORDER="1" BGCOLOR="white">...</TD>
 *                         <TD HEIGHT="30" WIDTH="10" ALIGN="right" BORDER="0"></TD>
 *                 </TR>
 *         </TABLE>>];
 * }
 * \enddot
 */
struct super_block {
	u32	magic;		  /**< Magic number */
	u32	nr_inodes;	  /**< How many inodes */
	u32	nr_sects;	  /**< How many sectors (including bit maps) */
	u32	nr_imap_sects;	  /**< How many inode-map sectors */
	u32	nr_smap_sects;	  /**< How many sector-map sectors */
	u32	n_1st_sect;	  /**< Number of the 1st data sector */

	u32	nr_inode_sects;   /**< How many inode sectors */
	u32	root_inode;       /**< Inode nr of root directory */
	u32	inode_size;       /**< INODE_SIZE */
	u32	inode_isize_off;  /**< Offset of `struct inode::i_size' */
	u32	inode_start_off;  /**< Offset of `struct inode::i_start_sect' */
	u32	dir_ent_size;     /**< DIR_ENTRY_SIZE */
	u32	dir_ent_inode_off;/**< Offset of `struct dir_entry::inode_nr' */
	u32	dir_ent_fname_off;/**< Offset of `struct dir_entry::name' */

	/*
	 * the following item(s) are only present in memory
	 */
	int	sb_dev; 	/**< the super block's home device */
};

/**
 * @def   SUPER_BLK_MAGIC_V1
 * @brief Magic number of super block, version 1.
 * @attention It must correspond with boot/include/load.h::SB_MAGIC_V1
 */
#define	SUPER_BLK_MAGIC_V1		0x111

/**
 * @def   SUPER_BLOCK_SIZE
 * @brief The size of super block in the device.
 *
 * Note that this is the size of the struct in the device, NOT in memory.
 * The size in memory is larger because of some more members.
 */
#define	SUPER_BLOCK_SIZE	56

/**
 * @struct inode
 * @brief  i-node
 *
 * The file, currently, have tree valid attributes only:
 *   - size
 *   - start_sect
 *   - nr_sects
 *
 * The start_sect and nr_sects locate the file in the device,
 * and the size show how many bytes is used.
 * If size < (nr_sects * SECTOR_SIZE), the rest bytes
 * are wasted and reserved for later writing.
 *
 * NOTE: Remember to change INODE_SIZE if the members are changed
 */
struct inode {
	u32	i_mode;		/**< Accsess mode. Unused currently */
	u32	i_size;		/**< File size */
	u32	i_start_sect;	/**< The first sector of the data */
	u32	i_nr_sects;	/**< How many sectors the file occupies */
	u8	_unused[16];	/**< Stuff for alignment */

	/* the following items are only present in memory */
	int	i_dev;
	int	i_cnt;		/**< How many procs share this inode  */
	int	i_num;		/**< inode nr.  */
};

/**
 * @def   INODE_SIZE
 * @brief The size of i-node stored \b in \b the \b device.
 *
 * Note that this is the size of the struct in the device, \b NOT in memory.
 * The size in memory is larger because of some more members.
 */
#define	INODE_SIZE	32
#define	MAX_FILENAME_LEN	12

/**
 * @struct dir_entry
 * @brief  Directory Entry
 */
struct dir_entry {
	int	inode_nr;		/**< inode nr. */
	char	name[MAX_FILENAME_LEN];	/**< Filename */
};

/**
 * @def   DIR_ENTRY_SIZE
 * @brief The size of directory entry in the device.
 *
 * It is as same as the size in memory.
 */
#define	DIR_ENTRY_SIZE	sizeof(struct dir_entry)

/**
 * @struct file_desc
 * @brief  File Descriptor
 */
struct file_desc {
	int		fd_mode;	/**< R or W */
	int		fd_pos;		/**< Current position for R/W. */
	struct inode*	fd_inode;	/**< Ptr to the i-node */
};

/*buffer for FS:5M~6M
*/
//extern	u8* fsbuf;
//extern	const int	FSBUF_SIZE;
#define FSBUF 0x500000
#define FSBUF_SIZE 0x100000

/* make device number from major and minor numbers */
#define	MAJOR_SHIFT		8
#define	MAKE_DEV(a,b)		((a << MAJOR_SHIFT) | b)
/* separate major and minor numbers from device number */
#define	MAJOR(x)		((x >> MAJOR_SHIFT) & 0xFF)
#define	MINOR(x)		(x & 0xFF)

#define INVALID_DRIVER	-20
#define	INVALID_INODE	0
#define ROOT_INODE              1

/* INODE::i_mode (octal, lower 12 bits reserved) */
#define I_TYPE_MASK     0170000
#define I_REGULAR       0100000
#define I_BLOCK_SPECIAL 0060000
#define I_DIRECTORY     0040000
#define I_CHAR_SPECIAL  0020000
#define I_NAMED_PIPE	0010000

#define	is_special(m)	((((m) & I_TYPE_MASK) == I_BLOCK_SPECIAL) ||	\
			 (((m) & I_TYPE_MASK) == I_CHAR_SPECIAL))

#define	NR_DEFAULT_FILE_SECTS	2048 /* 2048 * 512 = 1MB */
	
#endif /* _ORANGES_FS_H_ */
