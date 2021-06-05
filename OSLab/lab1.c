#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define msgtimes 10
void fhandler(int signum);
void phandler(int signum);

pid_t p1;
pid_t p2;
int pipfd[2];
int countp = 0;
int count1 = 0;
int count2 = 0;

int main ()
{
    if(pipe(pipfd) < 0){
        perror("pipe");
        exit(-1);
    }

    printf("Pipe has been created\n");

    if((p1=fork()) == 0) //子进程1
    { 
        signal(SIGINT,SIG_IGN);
        signal(SIGUSR1,phandler);
        char readbuf1[22];
        while(1)
        {
            read(pipfd[0],readbuf1,sizeof(readbuf1));
            printf("Child1:%s",readbuf1);
            count1++;
        }
    }
    else if((p2=fork()) == 0)   //子进程2
    {   
        signal(SIGINT,SIG_IGN);
        signal(SIGUSR2,phandler);
        char readbuf2[22];
        while (1)
        {
            read(pipfd[0],readbuf2,sizeof(readbuf2));
            printf("Child2:%s",readbuf2);
            count2++;
        }
        
    }
    else //父进程
    {     
        char writebuf[22];
        signal(SIGINT,fhandler);
        while(1)
        {
            if(msgtimes == countp)
            {
                kill(getpid(),SIGINT);
            }
            sprintf(writebuf,"I send you %d times.\n",++countp);
            write(pipfd[1],writebuf,sizeof(writebuf));
            sleep(1);
        }
    }
    
    return 0;
}

void fhandler(int signum)
{
    int status;
    printf("Parent process has caputured signal %d\n",signum);
    kill(p1,SIGUSR1);
    kill(p2,SIGUSR2);
    waitpid(p1,&status,0);
    waitpid(p2,&status,0);
    close(pipfd[1]);
    printf("Parent process has send %d messages\n",countp);
    exit(0);
}

void phandler(int signum)
{
    if(signum == SIGUSR1)
    {
        printf("Child process1 has received %d messages\n",count1);
        printf("Child process1 is killed by parent\n");
    }
    else if(signum == SIGUSR2)
    {
        printf("Child process2 has received %d messages\n",count2);
        printf("Child process2 is killed by parent\n");
    }
    close(pipfd[0]);
    exit(0);
}