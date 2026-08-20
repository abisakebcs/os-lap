#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
int main()
{
    int n = 0;
    int sum=0,evenSum=0,oddSum=0,j,k,i;
    printf("n = ");
    scanf("%d",&n);
    pid_t pid = fork();

    if (pid == 0){
       printf("P2: PID=%d Parent=%d\n", getpid(), getppid());
       for(i=0;i<=n;i++)
       {
	  if(i%2!=0)
	     oddSum+=i;
       }

	  printf("Child(p2) Odd Sum = %d\n",oddSum);
    }
    else if (pid > 0){
       printf("P1: PID=%d Parent=%d\n", getpid(), getppid());
       for(k=0;k<=n;k++)
       {
	  sum+=k;
       }
       printf("Parent(p1) Sum = %d\n",sum);
       pid = fork();

       if (pid == 0){
          printf("P3: PID=%d Parent=%d\n", getpid(), getppid());
	  for(j=0;j<=n;j++)
	  {
	     if(j%2==0)
		evenSum+=j;
	  }
	  printf("Child(p3) Even Sum = %d\n",evenSum);
       }

       else wait(NULL);
    }

    return 0;
}

