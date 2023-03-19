#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>


#define send_key 451
#define recv_key 452
#define MSGSZ 128
#define RMSGSZ MSGSZ + sizeof(ssize_t)

typedef struct msgbuf
{
    long mtype;
    ssize_t msize;
    char mtext[MSGSZ];
} msgbuf;

int main(int argc, char ** argv) {
    if (argc < 3) {
        printf("Usage: proc1 <input> <output>\n");
        exit(0);
    }
    int fin;
    int fout;
    int send_qid;
    int recv_qid;
    size_t buffer_len;
    ssize_t read_bytes;
    ssize_t written_bytes;
    struct msqid_ds qinfo;
    msgbuf send_buffer;
    msgbuf recv_buffer;
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

    if ((fin = open(argv[1], O_RDONLY)) < 0) {
        printf("Can\'t open input file\n");
        exit(-1);
    }
    if ((fout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR)) < 0) {
        printf("Can\'t create output file\n");
        exit(-11);
    }

    while ((read_bytes = read(fin, buffer, MSGSZ)) > 0) {
        send_buffer.mtype = 1;
        send_buffer.msize = read_bytes;
        memcpy(send_buffer.mtext, buffer, read_bytes);
        if (msgsnd(send_qid, &send_buffer, RMSGSZ, IPC_NOWAIT) < 0) {
            printf("Error sending message!\n");
            close(fin);
            close(fout);
            exit(-1);
        }
        printf("Sent message of %ld bytes\n",read_bytes);
    }
    close(fin);
    int qmsgsc = 0;
    printf("Waiting for second process...\n");
    do {
        if (msgrcv(recv_qid, &recv_buffer, RMSGSZ, 2, 0) < 0) {
            printf("Error recieving message!\n");
            close(fin);
            close(fout);
            exit(-1);
        }
        written_bytes = write(fout, recv_buffer.mtext, recv_buffer.msize);
        printf("Recv message of %ld bytes\n",written_bytes);
        // get number of messages in queue
        if ((msgctl(recv_qid, IPC_STAT, &qinfo)) < 0) {
            printf("Error getting info about recv queue!\n");
            exit(-1);
        }
        qmsgsc = (int) qinfo.msg_qnum;
        printf("Messages left: %d\n", qmsgsc);
    } while (qmsgsc > 0);
    close(fout);
    return 0;
}