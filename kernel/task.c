
#include"ipc.h"
#include"stdio.h"
#include"hd.h"
#include"misc.h"
//#include"ipc.h"
void task_fs(){
	printl("task_fs start");
	MESSAGE msg;
	msg.type=DEV_OPEN;
	send_recv(BOTH,TASK_HD,&msg);
	
	spin("FS");
}
