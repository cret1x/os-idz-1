#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>


const int buf_size = 5000;

int main(int argc, char ** argv) {
    int fin;
    int fout;
    int fifo_desc;
    char* pipe_read_calc = "prc.fifo";
    char* pipe_calc_write = "pcw.fifo";
    char buffer[buf_size];
    ssize_t read_bytes;
    ssize_t written_bytes;

    mknod(pipe_read_calc, S_IFIFO | 0666, 0);
    mknod(pipe_calc_write, S_IFIFO | 0666, 0);

   	if((fifo_desc = open(pipe_read_calc, O_RDONLY)) < 0){
	    printf("[PROC]: Can\'t open FIFO for reading\n");
	    exit(-1);
	}
	read_bytes = read(fifo_desc, buffer, buf_size);
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
	close(fifo_desc);
	if((fifo_desc = open(pipe_calc_write, O_WRONLY)) < 0){
	    printf("[PROC]: Can\'t open FIFO for writting\n");
	    exit(-1);
	}
	printf("[PROC]: Writing to pipe %ld bytes\n", read_bytes);
	written_bytes = write(fifo_desc, buffer, read_bytes);
	close(fifo_desc);
	printf("[PROC]: Finished job\n");
    return 0;
}
