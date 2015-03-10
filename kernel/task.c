
#include"ipc.h"
#include"stdio.h"
#include"hd.h"
#include"misc.h"
#include"proc.h"
//#include"ipc.h"
/*leave this for sometime
void task_fs(){
	printl("task_fs start");
	MESSAGE msg;
	msg.type=DEV_OPEN;
	send_recv(BOTH,TASK_HD,&msg);
	
	spin("FS");
}
*/
