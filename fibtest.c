#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include<fcntl.h>
#include <pthread.h>  
#include <sys/time.h>
#include <sys/resource.h>


//pre declaration of function to be used
long Fibonacci(long Whichnumber);

//pthread create function
void *Fib(void *recievedInt){
    extern int numberOfThreads;
    pthread_detach(pthread_self());  
    //sets the recieved int to rInts, and make it a long
    long rInts = (long)recievedInt;   
   
    //using seperate function for calculation bc idk how with the void stuff from pthreadscreate
    long ret = Fibonacci(rInts);
    
    printf("Fibonacci %ld is %ld\n", rInts, ret);
    numberOfThreads--;
    pthread_exit(NULL);	
   

}
//fib calculation function
long Fibonacci(long WhichNumber) {

//----If the 1st or second number, return the answer directly
    if (WhichNumber <= 1) {
        return(WhichNumber);
//----Otherwise return the sum of the previous two Fibonacci numbers
    } else {
        return(Fibonacci(WhichNumber - 2) + Fibonacci(WhichNumber - 1));
    }
}
int main(int argc,char *argv[]) {
    //getting cpu usage at start and end, measuring in between
    struct rusage usage;
    struct timeval start, end;
    getrusage(RUSAGE_SELF, &usage);
    start = usage.ru_utime;

    //clean up child
    int status;
    
    //defining number of threads int
    extern int numberOfThreads;

    //making the fifo FIBOPIPE and checking error
    if(mkfifo("FIBOPIPE", 0777) == -1){
        if(errno != EEXIST){
            printf("Can't make fifo!!!!!!!!!!");
            return 1;
        }
    }
    //creating child
    int ChildPID;
    ChildPID = fork();

    //fork and exec to run interface program
    if(ChildPID == 0){
        execlp("./FiboInter", NULL); 
    }
    //opening the pipe for reading and checking for problems
    int fd = open("FIBOPIPE", O_RDONLY);
    if(fd == -1){
        return 1;
    }
    //ints and threads used
    int nread, rc;
    int numEntered;
    pthread_t  thread_id;
    //loop waiting until the write program is finished
    while(fd != EOF){
        nread = read(fd, &numEntered, sizeof(int));
        printf("Recieved from FIFO: %d\n", numEntered);
        numberOfThreads++;
        //create the thread 
       rc= pthread_create(&thread_id, NULL, Fib,(void*)numEntered); 
       if(rc){
            printf("\n ERROR: return code from pthread_create is %d \n", rc);
            exit(1);
       }
       //when true, the interface is done
        if(nread <=0){
            //had to subtract one because it was counting the 0 as a thread
            numberOfThreads--;
            while(numberOfThreads > 0){
                printf("Waiting for %d threads to finish\n", numberOfThreads); 
                sleep(1);        
            } 
            break;
        }
         
    }
    //getting the usage a second time after the program is done. I know its different than the examples given in class but i like it this way
    getrusage(RUSAGE_SELF, &usage);
    end = usage.ru_utime;
    printf("Started at: %ld.%ds CPU time\n", start.tv_sec, start.tv_usec);
    printf("Ended at: %ld.%ds CPU time\n", end.tv_sec, end.tv_usec);

    //kills the child when done
    ChildPID = wait(&status);
    printf("Child %d has exited with status %d\n",ChildPID,status);

    //closing the pipe
    close(fd);

    //deleting the created FIFO
    unlink("FIBOPIPE");
    
}
//nOT variable
int numberOfThreads;