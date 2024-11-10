#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>

#define PORT 8080
#define SIZE 1024

char *response = "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n"
    "\r\n"
    "<html>"
	"<head><label>Test webserver on C</label></head>"
	"<body><h1>Hello, World!</h1></body></html>";


void clienter(int client_fd){
char buffer[SIZE];
int reader = recv(client_fd,buffer,SIZE-1,0);
if (reader < 0){
printf("Reading error\n");
close(client_fd);
return;
}

buffer[reader] = '\0';
printf("Request:\n%s\n",buffer);

send(client_fd,response,strlen(response),0);
close(client_fd);
}


int main(int argc, char *argv[]){
int port = 8080;
if (argc > 1){
char *endptr;
long porto = strtol(argv[1],&endptr,10);
port = (int)porto;
}

int server_fd,client_fd,addrlen,reader;
struct sockaddr_in addr;
addrlen = sizeof(addr);
char buffer[SIZE];
server_fd = socket(AF_INET,SOCK_STREAM,0);

if (server_fd < 0){
printf("Error creating server_fd\n");
return -1;
}

addr.sin_family = AF_INET;
addr.sin_addr.s_addr = inet_addr("127.0.0.1");
addr.sin_port = htons(port);

if (bind(server_fd,(struct sockaddr*)&addr,addrlen) < 0){
    printf("Error binding\n");
    return -1;
}

if (listen(server_fd,10) < 0){
    printf("Error listening\n");
    return -1;
}

while(1){

    client_fd = accept(server_fd,(struct sockaddr*)&addr,(socklen_t*)&addrlen);

    if (client_fd < 0){
        printf("Error creating client_fd\n");
        return -1;
        continue;
    }

    if (fork() == 0){
        close(server_fd);
        clienter(client_fd);
        exit(0);
    } else {
        close(client_fd);
    }
}

close(server_fd);
printf("Exit\n");
return 0;
}

