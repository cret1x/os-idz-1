#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

// swapped keys!!!!!!
#define recv_key 451
#define send_key 452
#define MSGSZ 128
#define RMSGSZ MSGSZ + sizeof(ssize_t)

typedef struct msgbuf
{
    long mtype;
    ssize_t msize;
    char mtext[MSGSZ];
} msgbuf;

int main(int argc, char ** argv) {
    int send_qid;
    int recv_qid;
    int qmsgsc = 1;
    size_t buffer_len;
    ssize_t read_bytes;
    ssize_t written_bytes;
    msgbuf send_buffer;
    msgbuf recv_buffer;
    struct msqid_ds qinfo;
    char buffer[MSGSZ];
    // create queue
    if((send_qid = msgget(send_key, IPC_CREAT | 0666)) < 0) {
        printf("Error creating seding queue!\n");
        exit(-1);
    } else {
        printf("Queue created id = %d\n", send_qid);
    }
    if((recv_qid = msgget(recv_key, IPC_CREAT | 0666)) < 0) {
        printf("Error creating recieving queue!\n");
        exit(-1);
    } else {
        printf("Queue created id = %d\n", recv_qid);
    }

    printf("Waiting for first process...\n");
    do {
        if (msgrcv(recv_qid, &recv_buffer, RMSGSZ, 1, 0) < 0) {
            printf("Error recieving message!\n");
            exit(-1);
        }
        printf("Recv message of %ld bytes\n", recv_buffer.msize);
        memcpy(buffer, recv_buffer.mtext, recv_buffer.msize);
        for (int i = 0; i < recv_buffer.msize; i++) {
            if ((buffer[i] >= 'A' && buffer[i] <= 'Z') || (buffer[i] >= 'a' && buffer[i] <= 'z')) {
                if (buffer[i] != 'e' && buffer[i] != 'y' && buffer[i] != 'u' && buffer[i] != 'i' && buffer[i] != 'o' && buffer[i] != 'a') {
                    if (buffer[i] >= 'a') {
                        buffer[i] -= 32;
                    }
                } 
            }
        }
        send_buffer.mtype = 2;
        send_buffer.msize = recv_buffer.msize;
        memcpy(send_buffer.mtext, buffer, recv_buffer.msize);
        if (msgsnd(send_qid, &send_buffer, RMSGSZ, IPC_NOWAIT) < 0) {
            printf("Error sending message!\n");
            exit(-1);
        }
        // get number of messages in queue
        if ((msgctl(recv_qid, IPC_STAT, &qinfo)) < 0) {
            printf("Error getting info about recv queue!\n");
            exit(-1);
        }
        qmsgsc = (int) qinfo.msg_qnum;
        printf("Messages left: %d\n", qmsgsc);
    } while (qmsgsc > 0);
    return 0;
}