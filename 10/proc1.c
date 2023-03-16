// 02-parent-child.c
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>


const int buf_size = 200;

int main(int argc, char ** argv) {
    if (argc < 3) {
        printf("Usage: proc1 <input> <output>\n");
        exit(0);
    }
    int fin;
    int fout;
    int fifo_desc1;
    int fifo_desc2;
    char* pipe_read_calc = "prc.fifo";
    char* pipe_calc_write = "pcw.fifo";
    char buffer[buf_size];
    char recv_buffer[buf_size];
    ssize_t read_bytes;
    ssize_t written_bytes;

    mknod(pipe_read_calc, S_IFIFO | 0666, 0);
    mknod(pipe_calc_write, S_IFIFO | 0666, 0);

    /* Reading Process */
    if((fifo_desc1 = open(pipe_read_calc, O_WRONLY)) < 0){
        printf("[READ+WRITE]: Can\'t open FIFO for writting\n");
        exit(-1);
    }
    printf("[READ+WRITE]: Reading from file %s...\n", argv[1]);
    fin = open(argv[1], O_RDONLY);
    if (fin < 0) {
        printf("[READ+WRITE]: Can\'t open file\n");
        exit(1);
    }
    fout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
    if (fout < 0) {
        printf("[READ+WRITE]: Can\'t create file\n");
        exit(1);
    }
    if((fifo_desc2 = open(pipe_calc_write, O_RDONLY)) < 0){
        printf("[READ+WRITE]: Can\'t open FIFO for reading\n");
        exit(-1);
    }

    while ((read_bytes = read(fin, buffer, buf_size)) > 0) {
        printf("[READ+WRITE]: Writing to pipe %ld bytes\n", read_bytes);
        written_bytes = write(fifo_desc1, buffer, read_bytes);
        read_bytes = read(fifo_desc2, recv_buffer, written_bytes);
        printf("[READ+WRITE]: Writing to file %s %ld bytes\n", argv[2], read_bytes);
        written_bytes = write(fout, recv_buffer, read_bytes);
    }
    close(fin);
    close(fout);
    close(fifo_desc1);
    close(fifo_desc2);
    printf("[READ+WRITE]: Finished job\n");
    return 0;
}