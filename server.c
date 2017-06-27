///COPIED CODE///
//SOURCE: day03/unitcp.c
//AUTHOR: Liam Echlin
//DESCRIPTION: Sample TCP server.
//NOTE: Modified for this author's uses and purposes.
// This whole file comes from day03/unitcp.c
#define _GNU_SOURCE

#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "filehandler.c"

// The next 39 lines come from the day02/udp_server
int main(int argc, char *argv[])
{
    if(argc != 2) {
        fprintf(stderr, "%s <working directory>\n", argv[0]);
        return 1;
    }

    int test = chdir(argv[1]);
    if(test != 0)
    {
        puts("Invalid directory.");
        exit(1);
    }

    // Port numbers are in the range 1-65535, plus null byte
    char port_num[8];
    snprintf(port_num, sizeof(port_num), "%hu", getuid());

    struct addrinfo *results;
    struct addrinfo hints = {0};
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int err = getaddrinfo("localhost", port_num, &hints, &results);
    if(err != 0) {
        fprintf(stderr, "Could not parse address: %s\n", gai_strerror(err));
        return 2;
    }

    int sd = socket(results->ai_family, results->ai_socktype, 0);
    if(sd < 0) {
        perror("Could not create socket");
        freeaddrinfo(results);
        return 3;
    }

    int set = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));

    err = bind(sd, results->ai_addr, results->ai_addrlen);
    if(err < 0) {
        perror("Could not bind");
        close(sd);
        freeaddrinfo(results);
        return 4;
    }
    freeaddrinfo(results);

    // 5 is the usual backlog
    err = listen(sd, 5);
    if(err < 0) {
        perror("Could not listen");
        freeaddrinfo(results);
        return 4;
    }

    struct sigaction ignorer = {0};
    ignorer.sa_handler = SIG_IGN;
    sigaction(SIGCHLD, &ignorer, NULL);

    // Next 36 lines also pulled from say02/udp_server
    for(;;) {
        // Picked an arbitrary size for example purposes
        char buf[256];
        struct sockaddr_storage client;
        socklen_t client_sz = sizeof(client);
        char ip[INET6_ADDRSTRLEN];
        unsigned short port;

        ///COPIED CODE////
        //SOURCE: https://stackoverflow.com/questions/1543466/how-do-i-change-a-tcp-socket-to-be-non-blocking
        //AUTHOR: "Matt"
        //DESCRIPTION: make a nonblocking socket with only one function call. Returns control to the code, allowing more requests, without perpetually trying to load more data.
        int remote = accept4(sd, (struct sockaddr *)&client, &client_sz, SOCK_NONBLOCK);
        ////END OF COPIED CODE/////.
        if(remote < 0) {
            perror("Could not accept connection");
            continue;
        }

        pid_t child = fork();

        // The child process handles the request
        if(child == 0) {
            close(sd);

            if(client.ss_family == AF_INET6) {
                inet_ntop(client.ss_family,
                        &((struct sockaddr_in6 *)&client)->sin6_addr,
                        ip, sizeof(ip));
                port = ntohs(((struct sockaddr_in6 *)&client)->sin6_port);
            } else {
                inet_ntop(client.ss_family,
                        &((struct sockaddr_in *)&client)->sin_addr,
                        ip, sizeof(ip));
                port = ntohs(((struct sockaddr_in *)&client)->sin_port);
            }

            printf("%s:%hu is connected\n", ip, port);

            ssize_t received = recv(remote, buf, sizeof(buf)-1, 0);
            char * file = file_name(buf);

            while(received >= 0) {
                buf[received] = '\0';
                //THIS SECTION IS NEW/MODIFIED BY JESSE OBERBECK.
                printf("SERVER BUF:%s", buf);
                
                printf("<<FILE>> = %s\n", file);
                
                //END SECTION
                received = recv(remote, buf, sizeof(buf)-1, 0);
                puts("STILL RECIEVING");
            }
if(strncmp(file, "cgi", 3) == 0)
                {
                    printf("COMPARE SUCCESS!\n");
                    cgi_file(file, remote);
                    free(file);
                }
                else if(strncmp(file, "404", 3) == 0)
                {
                    not_found(file, remote);
                }

                else if(strncmp(file, "HTTP", 4) == 0)
                {
                    puts("HTTP MATCH");
                    char * index = "www/index.html";
                    html_file(index, remote);
                }

                else
                {
                    html_file(file, remote);
                }
            if(received < 0) {
                perror("Problem receiving");
            }

            close(remote);
            return 0;
        }
        else if(child < 0) {
            perror("Could not spawn worker");
            continue;
        }

        close(remote);
    }
}
///END OF COPIED CODE/////.
