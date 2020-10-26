/* Echo SERVER */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/types.h>

#define BUFSIZE 1024
#define MAXCLIENT 10

// accept()한 client 처리 스레드 함수
void *clientHandler(void *arg);

int main(int argc, char *argv[])
{
    // server, client sock var
    int serv_sock;
    int clnt_sock;

    // server, client addr var
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    int clnt_addr_size = sizeof(clnt_addr); // cnlt size

    // thread id var
    pthread_t t_id;

    // Use Explanation
    if (argc != 2)
    {
        fprintf(stderr, "Usage:%s <port>\n", argv[0]);
        return 0;
    }

    // get socket
    if ((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket() error!!");
        return -1;
    }

    // struct initiating
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    // binding addr to socket
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("bind() error!!");
        return -1;
    }

    // waiting until 3 client connect
    if (listen(serv_sock, MAXCLIENT) == -1)
    {
        perror("listen() error!!");
        return -1;
    }

    fprintf(stderr, "Waiting for Clients...\n");

    // execute client's connection
    while (1)
    {
        // accpet client's connection
        if ((clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size)) == -1)
        {
            perror("accept() error!!");
            return -1;
        }

        // create a thread and hand over client's socket
        if (pthread_create(&t_id, NULL, clientHandler, (void *)&clnt_sock) < 0)
        {
            perror("pthread_create() error!!");
            return -1;
        }
    }
    close(serv_sock);

    return 0;
}

/* func to execute accepted clients */
void *clientHandler(void *arg)
{
    fprintf(stderr, "A Client Has Been Connected\n");

    // detach a thread from main thread
    pthread_detach(pthread_self());

    // buf to save message, client's sock num
    char buf[BUFSIZE];
    int clnt_sock = *((int *)arg); // typecasting void to int
    int n_read;

    // message read from clinets
    while ((n_read = read(clnt_sock, buf, BUFSIZE - 1)) > 0)
    {
        write(STDOUT_FILENO, buf, n_read);

        // return msg to clnt
        if (write(clnt_sock, buf, n_read) < 0)
        {
            perror("write() error!!");
            exit(1);
        }
    }

    // close socket and end thread
    close(clnt_sock);
    return (void *)NULL;
}
