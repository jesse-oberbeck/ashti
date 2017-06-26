#define _GNU_SOURCE

#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

void html_file(char *fname, int socket)
{
    char buffer[128];
    FILE *file = fopen(fname, "r");
    //TODO: turn prints into sends.

    char * head1 = "HTTP:/1.1 200 OK\n";
    //printf("HTTP:/1.1 200 OK\n");
    send(socket, head1, strlen(head1), 0);

    char * head2 = "Content-type: text/html\n\n";
    //printf("Content-type: text/html\n");
    send(socket, head2, strlen(head2), 0);


    while(fgets(buffer, 128, file))
    {
        //TODO: send each piece here rather than just print it.
        printf("%s\n", buffer);
        send(socket, buffer, strlen(buffer), 0);
    }
}

char * file_name(char * data)
{
    char *buf;
    char *cmp = "GET";

    buf = strtok(data, " ");

    printf("<<<<<<<<<<<<BUFCHECK: %s\n", buf);
    if(strcmp(buf, cmp) == 0)
    {
        buf = strtok(NULL, "/ ");
        printf("Success! --%s--\n", buf);
    }

    else
    {
        printf("404\n");
        return(NULL);
    }

    if(access(buf, F_OK) != -1)
    {
        return(buf);
        //html_file(buf);
        //printf("FILE EXISTS!\n");
    }
    else
    {
        printf("404\n");
        return(NULL);
    }

}
/*
int handler_main(void)
{
    char test[128] = "GET /pack.txt";
    file_name(test);
}
*/
