#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
main()
{



 FILE *file1;
 int fifo_server,fifo_client;
 //char str[256];
 char buf = 0;
 int choice=1;
 printf("Choose the request to be sent to server from options below");
 printf("\n\t\t Enter 1 for O.S.Name \n \
                Enter 2 for Distribution \n \
                Enter 3 for Kernel version \n");
 scanf("%d",&choice);

 fifo_server=open("/tmp/keepawaykeyboardserver",O_RDWR);
 if(fifo_server < 0) {
  printf("Error in opening file");
  exit(-1);
  }

 write(fifo_server,&choice,sizeof(int));

 fifo_client=open("/tmp/keepawaykeyboardclient",O_RDWR);

 if(fifo_client < 0) {
  printf("Error in opening file");
  exit(-1);
  }

 //buf=malloc(sizeof(char));
 read (fifo_client,&buf,sizeof(char));
 printf("Got character %c, hex is 0x%x\n", buf, buf);
// printf("\n ***Reply from server is %s***\n",buf);
 close(fifo_server);
 close(fifo_client);
}

