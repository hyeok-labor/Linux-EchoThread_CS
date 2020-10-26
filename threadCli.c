/* Echo CLIENT */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    // sock num, msg leng var
    int sock;
    int str_len, recv_len, recv_num;

    // msg buf, server addr var
    char message[BUFSIZE];
    struct sockaddr_in serv_addr;

    // Use Explanation
    if (argc != 3)
    {
        fprintf(stderr, "Usage:%s <ip> <port>\n", argv[0]);
        return 0;
    }

    // socket allocation
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket() error!!");
        return -1;
    }

    // struct initiating
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("connect() error!!");
        return -1;
    }
    fprintf(stderr, "Connected To Server(%s:%s)\n", argv[1], argv[2]);

    while (1)
    {
        fprintf(stderr, "Message To Server (q to quit):");
        str_len = read(STDIN_FILENO, message, BUFSIZE - 1);
        message[str_len] = 0;

        // "q" ==> client exit
        if (!strcmp(message, "q\n"))
            break;
        // send msg to Server
        write(sock, message, str_len);

        // read msg until recv msg is exactly same with send msg
        for (recv_len = 0; recv_len < str_len;)
        {
            if ((recv_num = read(sock, message + recv_len, str_len - recv_len)) == -1)
            {
                perror("read() error!!");
            }
            recv_len += recv_num;
        }
        message[str_len] = 0;

        // print msg that got returned
        fprintf(stderr, "Message From Server:%s\n", message);
    }

    // close sock and end program
    close(sock);
    return 0;
}