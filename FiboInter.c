#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include<fcntl.h>


int main(int argc,char *argv[]) {
    //open fifo for writing only
    int fd = open("FIBOPIPE", O_WRONLY);
    if(fd == -1){
        return 1;
    }
    //prompt user for number, and loops until a 0 is entered
    int num;
    printf("What Fibonacci number do you want: \n");
    while(scanf("%d", &num) == 1 && num != 0){
        if(write(fd, &num, sizeof(int)) == -1){
            return 2;
        }
       
        
        printf("What Fibonacci number do you want: \n");
}
     

    close(fd);
    return 0;
}