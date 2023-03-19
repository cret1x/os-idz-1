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
    if (argc < 3) {
        printf("Usage: proc1 <input> <output>\n");
        exit(0);
    }
    int msgid;
    size_t buf_len;
    msgbuf sbuf;
    msgid = msgget(queue_key, IPC_CREAT | 0666);
    printf("Queue created id = %d\n", msgid);
    sbuf.mtype = 1;
    strcpy(sbuf.mtext, "Fuck that shit");
    buf_len = strlen(sbuf.mtext) + 1;

    if (msgsnd(msgid, &sbuf, buf_len, IPC_NOWAIT) < 0) {
        printf("Error\n");
    }
    printf("Sent! - %s\n",sbuf.mtext);

    return 0;
}