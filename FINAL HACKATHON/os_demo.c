#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Small, bounded demonstrations of real Linux process/system-programming concepts. */
static void show_tree(void) {
    printf("=== Linux process tree (ps) ===\n");
    fflush(stdout);
    execlp("ps", "ps", "-eo", "pid,ppid,stat,comm", "--forest", (char *)NULL);
    perror("execlp ps");
    exit(1);
}

static void demo_fork(void) {
    printf("=== fork(): parent creates a child ===\n");
    printf("Before fork: current PID=%ld, PPID=%ld\n",(long)getpid(),(long)getppid());
    fflush(stdout);
    pid_t pid=fork();
    if(pid<0){perror("fork");exit(1);}
    if(pid==0){
        printf("[child] PID=%ld, PPID=%ld\n",(long)getpid(),(long)getppid());
        _exit(0);
    }
    printf("[parent] PID=%ld created child PID=%ld\n",(long)getpid(),(long)pid);
    int status;
    if(waitpid(pid,&status,0)<0){perror("waitpid");exit(1);}
    printf("Parent collected child exit status. Child exited=%s\n",WIFEXITED(status)?"yes":"no");
}

static void demo_exec(void) {
    printf("=== exec(): replace this process with a program ===\n");
    printf("Current PID before exec=%ld\n",(long)getpid());
    fflush(stdout);
    execlp("echo","echo","exec started the echo program successfully",(char *)NULL);
    perror("execlp echo");
    exit(1);
}

static void demo_wait(void) {
    printf("=== waitpid(): parent waits for child ===\n");
    fflush(stdout);
    pid_t pid=fork();
    if(pid<0){perror("fork");exit(1);}
    if(pid==0){
        printf("[child] PID=%ld doing a short task\n",(long)getpid());
        fflush(stdout);
        _exit(7);
    }
    int status;
    printf("[parent] PID=%ld waiting for child PID=%ld\n",(long)getpid(),(long)pid);
    if(waitpid(pid,&status,0)<0){perror("waitpid");exit(1);}
    if(WIFEXITED(status)) printf("[parent] child finished with exit code %d\n",WEXITSTATUS(status));
}

static volatile sig_atomic_t signal_received=0;
static void handle_sigusr1(int sig){(void)sig;signal_received=1;}
static void demo_signal(void) {
    struct sigaction sa;
    memset(&sa,0,sizeof(sa)); sa.sa_handler=handle_sigusr1; sigemptyset(&sa.sa_mask);
    if(sigaction(SIGUSR1,&sa,NULL)<0){perror("sigaction");exit(1);}
    printf("=== POSIX signal: SIGUSR1 ===\n");
    printf("PID=%ld installs a signal handler\n",(long)getpid());
    fflush(stdout);
    if(kill(getpid(),SIGUSR1)<0){perror("kill");exit(1);}
    if(signal_received) printf("Signal received: custom handler ran successfully\n");
}

static void demo_ipc(void) {
    int fds[2];
    if(pipe(fds)<0){perror("pipe");exit(1);}
    printf("=== Anonymous pipe IPC ===\n");
    pid_t pid=fork();
    if(pid<0){perror("fork");close(fds[0]);close(fds[1]);exit(1);}
    if(pid==0){
        close(fds[1]);
        char buf[128]={0}; ssize_t n=read(fds[0],buf,sizeof(buf)-1);
        if(n>=0){buf[n]='\0';printf("[child] received message: %s\n",buf);fflush(stdout);}
        close(fds[0]);_exit(0);
    }
    close(fds[0]);
    const char *msg="Hello from parent through pipe";
    if(write(fds[1],msg,strlen(msg))<0)perror("write");
    close(fds[1]);
    int status; waitpid(pid,&status,0);
    printf("[parent] sent message and waited for child\n");
}

int main(int argc,char **argv) {
    setvbuf(stdout,NULL,_IONBF,0);
    if(argc!=2){fprintf(stderr,"Usage: %s tree|fork|exec|wait|signal|ipc\n",argv[0]);return 2;}
    if(strcmp(argv[1],"tree")==0)show_tree();
    else if(strcmp(argv[1],"fork")==0)demo_fork();
    else if(strcmp(argv[1],"exec")==0)demo_exec();
    else if(strcmp(argv[1],"wait")==0)demo_wait();
    else if(strcmp(argv[1],"signal")==0)demo_signal();
    else if(strcmp(argv[1],"ipc")==0)demo_ipc();
    else {fprintf(stderr,"Unknown demo: %s\n",argv[1]);return 2;}
    return 0;
}
