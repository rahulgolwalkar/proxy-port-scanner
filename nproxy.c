#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/poll.h>
#define MAX_THREADS 10     //Vary this number to create threads as many as u like
void *functionC();
char host[20];
char proxy[20];
char pport[4];
int port_state[1024]={0};
int main(int argc,char *argv[])
{
    int start=75,stop=82,i,j,temp;
    pthread_t thread_id[MAX_THREADS];
    int counter ;
    counter = start;
    if(argc != 6)
    {
        fprintf(stderr,"Usage: nproxy [proxy server] [proxy port] [dest. host] [start port range] [stop port range]");
        exit(0);
    }
    strcpy(proxy,argv[1]);       //Converting Args to preferrend names
    strcpy(pport,argv[2]);
    strcpy(host,argv[3]);
    start = atoi(argv[4]);
    stop = atoi(argv[5]);
    while(counter <= stop)      
    {
        for( i=0;i<MAX_THREADS && counter <= stop;i++)
        {

            pthread_create(&thread_id[i],NULL,&functionC, counter);     //Multithreading
            counter++;

        }
        temp = i;
        for(i=0;i<temp;i++)
        {
            pthread_join(thread_id[i],NULL);
        }


    }

    for(i=start;i<=stop;i++)
    {
        if(port_state[i]==1)
            fprintf(stderr,"\n%d    open",i);
        else
            fprintf(stderr,"\n%d    timeout",i);
    }

    return 0;

}
void *functionC(int ptr)       // function that scans a single port
{

    //fprintf(stderr,"======== %d",ptr);

    struct addrinfo hints , *res;
    struct pollfd  fds[1];
    int sockfd,k=0;

    char buf[100],msg[50],c_port[4];
    char *msg1 = " \r\n\r\n";
    int len, bytes_sent;

   /*Creating the String to CONNECT*/
    strcat(msg,"CONNECT ");     // msg = "CONNECT google.com:80 \r\n\r\n";
    strcat(msg,host);
    strcat(msg,":");
    sprintf(c_port,"%d",ptr);
    strcat(msg,c_port);
    strcat(msg,msg1);            // fprintf(stderr,"%s",msg);

    memset(&hints,0,sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM ;

    getaddrinfo(proxy,pport,&hints,&res);
    sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    fds[0].fd = sockfd;
    fds[0].events = POLLIN ;
    if(connect(sockfd,res->ai_addr,res->ai_addrlen)==-1)
        fprintf(stderr,"Relay server down\n");
    len = strlen(msg);
    bytes_sent = send(sockfd,msg,len,0);
    if(poll(fds,1,5000)>0)            //waiting for 5 secs for a reply
    {
        recv(sockfd,buf,99,0);
        if(strstr(buf,"200")!=NULL)
        {
             port_state[ptr]=1;      //fprintf(stderr,"open");
        }

    }
   else
   {}          //fprintf(stderr,"timeout");

}