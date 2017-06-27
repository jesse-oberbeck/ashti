#define _GNU_SOURCE

#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

void cgi_file(char *fname, int socket)
{
    char buffer[128];
    FILE *file = popen(fname, "r");

    char * head1 = "HTTP:/1.1 200 OK\n";
    send(socket, head1, strlen(head1), 0);

    //char * head2 = "Content-type: text/html\n\n";
    //send(socket, head2, strlen(head2), 0);


    while(fgets(buffer, 128, file))
    {
        printf("%s\n", buffer);
        send(socket, buffer, strlen(buffer), 0);
    }
}

void html_file(char *fname, int socket)
{
    char buffer[128];
    FILE *file = fopen(fname, "r");

    char * head1 = "HTTP:/1.1 200 OK\n";
    send(socket, head1, strlen(head1), 0);

    char * head2 = "Content-type: text/html\n\n";
    send(socket, head2, strlen(head2), 0);


    while(fgets(buffer, 128, file))
    {
        printf("%s\n", buffer);
        send(socket, buffer, strlen(buffer), 0);
    }
}

char * file_name(char * data)
{
    char *buf;
    char *get = "GET";
    char *cgi = "cgi-bin";
    char *cgi_bin = "cgi-bin/";
    char *www = "www/";
    int cgi_flag = 0;
    char *name_buf = calloc(128, 1);

    buf = strtok(data, " ");

    printf("<<<<<<<<<<<<BUFCHECK: %s\n", buf);
    if(strcmp(buf, get) == 0)
    {
        buf = strtok(NULL, "/ ");
            if(strcmp(buf, cgi) == 0)
            {
                buf = strtok(NULL, "/ ");
                printf("CGI Success! %s\n", buf);
                cgi_flag = 1;
            }
        printf("Success! %s\n", buf);
    }

    else
    {
        printf("404\n");
        return(NULL);
    }

    if(cgi_flag)
    {
        strncpy(name_buf, cgi_bin, strlen(cgi_bin));
        strncat(name_buf, buf, 128 - strlen(cgi_bin));
        printf("CAT RESULT! %s\n", name_buf);
        return(name_buf);
    }

    else if((access(buf, F_OK) != -1) && strcmp(buf, "www") != 0)
    {
        puts("ACCESS SUCCESS!");
        return(buf);
    }

    else if(strcmp(buf, "www") == 0)
    {
        buf = strtok(NULL, "/ ");
    }

    strncpy(name_buf, www, strlen(www));
    strncat(name_buf, buf, 128 - strlen(www));
    printf("NAME CHANGED FOR WWW: %s\n", name_buf);

    if(access(name_buf, F_OK) != -1)
    {
        puts("WWW ACCESS SUCCESS!");
        return(name_buf);
    }

    else
    {
        printf("404\n");
        return(NULL);
    }

}

