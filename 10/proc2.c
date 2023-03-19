#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>


#define queue_key 451
#define MSGSZ 128

typedef struct msgbuf
{
    long mtype;
    char mtext[MSGSZ];
} msgbuf;

int main(int argc, char ** argv) {
    
    int msgid;
    msgbuf sbuf;
    msgid = msgget(queue_key, 0666);
    printf("Queue opened id = %d\n", msgid);
    if (msgrcv(msgid, &sbuf, MSGSZ, 1, 0) < 0) {
        printf("Error\n");
    }
    printf("RECV! - %s\n",sbuf.mtext);

    return 0;
}