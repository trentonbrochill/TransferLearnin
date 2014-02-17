#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <termios.h>

int g_latest_char;

pthread_t g_threads[1];

int fifo_server,fifo_client;

int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

void shutdown(int signum){

        close(fifo_server);
        close(fifo_client);

    if( access( "/tmp/keepawaykeyboardserver", F_OK ) != -1 ) {
        // file exists
        if( remove( "/tmp/keepawaykeyboardserver" ) != 0 )
            perror( "Error deleting server file" );
    }

    if( access( "/tmp/keepawaykeyboardclient", F_OK ) != -1 ) {
        // file exists
        if( remove( "/tmp/keepawaykeyboardclient" ) != 0 )
            perror( "Error deleting server file" );
    }
    pthread_kill(g_threads[0], SIGTERM);
    exit(0);
}

void* getch_loop(void* threadid)
{
    while (1)
    {
        int cur_char = getch();
        if (cur_char != 0x00)
        {
           // printf("Got character %c, hex is 0x%x\n", (char)cur_char, cur_char);
            g_latest_char = cur_char;
        } ;
    }
    return NULL;
}

main()
{

    // check if fifos exist
    if( access( "/tmp/keepawaykeyboardserver", F_OK ) != -1 ) {
        // file exists
        if( remove( "/tmp/keepawaykeyboardserver" ) != 0 )
            perror( "Error deleting server file" );
    }

    if( access( "/tmp/keepawaykeyboardclient", F_OK ) != -1 ) {
        // file exists
        if( remove( "/tmp/keepawaykeyboardclient" ) != 0 )
            perror( "Error deleting server file" );
    }


    int file1,file2;

    file1 = mkfifo("/tmp/keepawaykeyboardserver",0666);
    if(file1 < 0) {
        printf("Unable to create a fifo");
        exit(-1);
    }

    file2 = mkfifo("/tmp/keepawaykeyboardclient",0666);

    if(file1<0) {
        printf("Unable to creat./e a fifo");
        exit(-1);
    }
    printf("fifos server and child created successfuly");

    
    int choice;
    char *buf;

    // delete nice if ctrl-c
    signal(SIGINT, shutdown);

    // create some nice threads
    g_latest_char = 0x00;
    printf("Spawning input loop thread...\n");

    int child_tid = pthread_create(&g_threads[0], NULL, (void*)getch_loop, (void*)0);


        fifo_server = open("/tmp/keepawaykeyboardserver",O_RDWR);
        if(fifo_server<1) {
            printf("Error opening server");
        }


        fifo_client = open("/tmp/keepawaykeyboardclient",O_RDWR);

        if(fifo_client<1) {
            printf("Error opening client");
        }

    while(1){



        read(fifo_server,&choice,sizeof(int));





        int cur_char = g_latest_char;
        
        write(fifo_client,&cur_char,sizeof(char));
//printf("Sending %c, hex is 0x%x\n", (char)cur_char, cur_char);
        g_latest_char = 0x00;

//        switch(choice) {

//        case 1:
//            buf="Linux";
//            write(fifo_client,buf,10*sizeof(char));
//            printf("\n Data sent to client \n");
//            break;
//        case 2:

//            buf="debian";
//            write(fifo_client,buf,10*sizeof(char));
//            printf("\nData sent to client\n");
//            break;
//        case 3:
//            buf="2.6.32";
//            write(fifo_client,buf,10*sizeof(char));
//            printf("\nData sent to client\n");
//        }




    }


}
