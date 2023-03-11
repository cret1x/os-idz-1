// 02-parent-child.c
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>


const int buf_size = 5000;

int main(int argc, char ** argv) {
    if (argc < 3) {
        printf("Usage: main <input> <output>\n");
        exit(0);
    }
    int fin;
    int fout;
    int result;
    int pipe_read_calc[2];
    char buffer[buf_size];
    ssize_t read_bytes;
    ssize_t written_bytes;

    if (pipe(pipe_read_calc) < 0) {
        printf("Can\'t open pipe\n");
        exit(-1);
    }

    result = fork();
    if (result < 0) {
        printf("Can\'t fork child\n");
        exit(-1);
    } else if (result > 0) {
        /* Reading Process */
        printf("[READ]: Reading from file %s...\n", argv[1]);
        if (close(pipe_read_calc[0]) < 0) {
            printf("[READ]: Can\'t close reading side of pipe\n");
            exit(-1);
        }
        fin = open(argv[1], O_RDONLY);
        if (fin < 0) {
            printf("[READ]: Can\'t open file\n");
            exit(1);
        }
        read_bytes = read(fin, buffer, buf_size);
        if (read_bytes > 0) {
            printf("[READ]: Writing to pipe %ld bytes\n", read_bytes);
            written_bytes = write(pipe_read_calc[1], buffer, read_bytes);
            if (written_bytes != read_bytes) {
                printf("[READ]: Can\'t write all string to pipe\n");
                exit(-1);
            }
        }
        close(fin);
        if (close(pipe_read_calc[1]) < 0) {
            printf("[READ]: Can\'t close writing side of pipe\n");
            exit(-1);
        }
        printf("[READ]: Finished job\n");
    } else {
        result = fork();
        if (result < 0) {
            printf("Can\'t fork child\n");
            exit(-1);
        } else if (result > 0) {
            /* Writing process */
            if (close(pipe_read_calc[1]) < 0) {
                printf("[WRITE]: Can\'t close writing side of pipe\n");
                exit(-1);
            }
            fout = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR);
            if (fout < 0) {
                printf("[WRITE]: Can\'t create file\n");
                exit(1);
            }
            printf("[WRITE]: Reading from pipe...\n");
            read_bytes = read(pipe_read_calc[0], buffer, buf_size);
            printf("[WRITE]: Writing to file %s %ld bytes\n", argv[2], read_bytes);
            written_bytes = write(fout, buffer, read_bytes);
            close(fout);
            if (close(pipe_read_calc[0]) < 0) {
                printf("[WRITE]: Can\'t close reading side of pipe\n");
                exit(-1);
            }
            printf("[WRITE]: Finished job\n");
        } else {
            /* Processing process */
            read_bytes = read(pipe_read_calc[0], buffer, buf_size);
            printf("[PROC]: Processing string of %ld bytes...\n", read_bytes);
            for (int i = 0; i < read_bytes; i++) {
                if ((buffer[i] >= 'A' && buffer[i] <= 'Z') || (buffer[i] >= 'a' && buffer[i] <= 'z')) {
                    if (buffer[i] != 'e' && buffer[i] != 'y' && buffer[i] != 'u' && buffer[i] != 'i' && buffer[i] != 'o' && buffer[i] != 'a') {
                        if (buffer[i] >= 'a') {
                            buffer[i] -= 32;
                        }
                    } 
                }
            }
            printf("[PROC]: Writing to pipe %ld bytes\n", read_bytes);
            written_bytes = write(pipe_read_calc[1], buffer, read_bytes);
            if (close(pipe_read_calc[1]) < 0) {
                printf("[PROC]: Can\'t close writing side of pipe\n");
                exit(-1);
            }
            if (close(pipe_read_calc[0]) < 0) {
                printf("[PROC]: Can\'t close reading side of pipe\n");
                exit(-1);
            }
            printf("[PROC]: Finished job\n");
        }
    }
    return 0;
}