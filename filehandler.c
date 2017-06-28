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

void bad_request(int socket)
{
    //Sends 500 error.
    const char * head1 = "HTTP:/1.1 200 OK\n";
    send(socket, head1, strlen(head1), 0);

    const char * head2 = "Content-type: text/html\n\n";
    send(socket, head2, strlen(head2), 0);

    const char *nf_error = "<html><head><title>500 - request cannot be processed.</title><style></style></head><h1>404</h1><h2>PAGE NOT FOUND </h2></html>";
    printf("%s\n", nf_error);
    send(socket, nf_error, strlen(nf_error), 0);
}

void not_found(int socket)
{
    //Sends 404 error.
    const char * head1 = "HTTP:/1.1 200 OK\n";
    send(socket, head1, strlen(head1), 0);

    const char * head2 = "Content-type: text/html\n\n";
    send(socket, head2, strlen(head2), 0);

    const char *nf_error = "<html><head><title>404 - this page does not exist</title><style></style></head><h1>404</h1><h2>PAGE NOT FOUND </h2></html>";
    printf("%s\n", nf_error);
    send(socket, nf_error, strlen(nf_error), 0);
}

void cgi_file(char *fname, int socket)
{
    //Runs and sends the result of cgi scripts.
    char buffer[128];
    FILE *file = popen(fname, "r");

    const char * head1 = "HTTP:/1.1 200 OK\n";
    send(socket, head1, strlen(head1), 0);

    size_t read;
    while((read = fread(buffer, 1, 128, file)))
    {
        //printf("%s\n", buffer);
        send(socket, buffer, read, 0);
    }
    pclose(file);
}

void html_file(char *fname, int socket)
{
    //Opens and sends html files.
    puts("HTML START");
    char buffer[128];
    FILE *file = fopen(fname, "r");

    const char * head1 = "HTTP:/1.1 200 OK\n";
    send(socket, head1, strlen(head1), 0);

    const char * head2 = "Content-type: text/html\n\n";
    send(socket, head2, strlen(head2), 0);

    size_t read;
    while((read = fread(buffer, 1, 128, file)))
    {
        //printf("%s\n", buffer);
        send(socket, buffer, read, 0);
    }
    fclose(file);
}

char * file_name(char * data)
{
    //Extracts requested file name from GET, otherwise
    //defaults to index.html or 404.
    const char *buf = NULL;
    const char *get = "GET";
    const char *cgi = "cgi-bin";
    const char *cgi_bin = "cgi-bin/";
    const char *www = "www/";
    const char *http = "HTTP";
    const char *super_fail = "500";
    int cgi_flag = 0;
    char *name_buf = calloc(128, 1);
    if(data == NULL)
    {
        return((char *)super_fail);
    }
    printf("<<<<<<<<<<<<BUFCHECK: %s\n", buf);
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

    if(strncmp(buf, "HTTP", 4) == 0)
    {
        free(name_buf);
        return((char *)http);
    }

    else if(cgi_flag)
    {
        strncpy(name_buf, cgi_bin, strlen(cgi_bin));
        strncat(name_buf, buf, 128 - strlen(cgi_bin));
        return(name_buf);
    }

    else if((access(buf, F_OK) != -1) && strcmp(buf, "www") != 0)
    {
        puts("Normal http.");
        free(name_buf);
        return((char *)buf);
    }

    else if(strcmp(buf, "www") == 0)
    {
        buf = strtok(NULL, "/ ");
    }
    printf("before. %s\n", name_buf);
    strncpy(name_buf, www, strlen(www));
    strncat(name_buf, buf, 128 - strlen(www));
    printf("After. %s\n", name_buf);

    if(access(name_buf, F_OK) != -1)
    {
        printf("Access success. %s\n", name_buf);
        return(name_buf);
    }

    else
    {
        const char *fail = "404";
        free(name_buf);
        printf("404\n");
        return((char *)fail);
    }

    return((char *)super_fail);

}

