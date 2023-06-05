#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h> //Internet Protocol family, ex: sin_family, sin_port, ..., ect
#include <sys/socket.h>
#include <arpa/inet.h> //for inet_addr()
#include <math.h>
#include <unistd.h> //write(.)
#define SIZE 512

/*require (1.socket() 2.connect() 3.write())build up a TCP client*/

void split(char *input, char split_word[3][64]);
int main(void)
{
    struct sockaddr_in srv;
    struct sockaddr_in cli;
    int nbytes = 0;
    int fd = -1;
    int server_port = 0;
    char *server_ip = NULL;

    while (1)
    {
        char input[64] = {'\0'};
        char split_word[3][64] = {'\0'};
        char receiveMessage[64] = {'\0'};

        printf("[student @ CSE ~]$ ");
        fgets(input, 64, stdin);
        split(input, split_word);

        printf("input: %s\n", input);
        if (strcmp(split_word[0], "connect") == 0)
        {
            fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // AF_INET是指用IPv4 , SOCK_STREAM select the TCP protocol
            /*socket procedure*/
            if (fd < 0)
            {
                perror("socket error");
                exit(1); // 非正常運行檔至退出程序
            }

            server_ip = split_word[1];
            server_port = atoi(split_word[2]);
            /*connect procedure*/
            /* connect: use the Internet address family */
            srv.sin_family = AF_INET;
            /* connect: socket ‘fd’ to port */
            srv.sin_port = htons(server_port);
            /* connect: connect to IP address */
            srv.sin_addr.s_addr = inet_addr(server_ip); // inet_addr() 是將有.的ip轉為二進位
            if (connect(fd, (struct sockaddr *)&srv, sizeof(srv)) < 0)
            {
                perror("connect error");
                exit(1);
            }

            printf("The server with IP \"%s\" has accepted your connection\n", split_word[1]);
        }
        else if (strcmp(split_word[0], "add") == 0 | strcmp(split_word[0], "mul") == 0)
        {
            if (fd == -1)
            {
                printf("Please connect first\n");
                continue;
            }

            // 確認使用者有輸入 add x y
            if (strcmp(split_word[1], "") == 0 | strcmp(split_word[2], "") == 0)
            {
                printf("Please enter two numbers after the \'%s\' command\n", split_word[0]);
                continue;
            }

            // 確認 x y 是數字
            // if (isdigit(split_word[1]) != 0)
            // {
            //     printf("The input following the \'add\' command must be a number\n");
            //     continue;
            // }
            // if (isdigit(split_word[2]) != 0)
            // {
            //     printf("The input following the \'add\' command must be a number\n");
            //     continue;
            // }
            printf("Send: %s %s %s\n", split_word[0], split_word[1], split_word[2]);

            write(fd, input, sizeof(input));
            read(fd, &receiveMessage, sizeof(receiveMessage));
            printf("Get \'%s\' from server.\n", receiveMessage);
        }
        else if (strcmp(split_word[0], "abs") == 0)
        {
            if (fd == -1)
            {
                printf("Please connect first\n");
                continue;
            }

            // 確認使用者有輸入 abs x
            if (strcmp(split_word[1], "") == 0)
            {
                printf("Please enter one number after the \'abs\' command\n");
                continue;
            }

            printf("Send: %s %s\n", split_word[0], split_word[1]);

            write(fd, input, sizeof(input));
            read(fd, receiveMessage, sizeof(receiveMessage));
            printf("Get \'%s\' from server.\n", receiveMessage);
        }
        else if (strcmp(split_word[0], "kill") == 0)
        {
            printf("Goodbye.\n");
            write(fd, input, sizeof(input));
            close(fd);
            break;
        }
        else if (strcmp(split_word[0], "help") == 0)
        {
            printf("You can enter three commands to do something\n");
            printf("1. [connect server_ip server_port]:connect to server\n");
            printf("2. [add x y]: a+b\n");
            printf("3. [abs a]: |a|\n");
            printf("4. [mul x y]: a*b\n");
            printf("5. [kill]:close the connection\n");
        }
        else
        {
            // printf("please enter correct command!!! or you can enter \"help\" to see correct commands\n");
            write(fd, input, sizeof(input));
            read(fd, receiveMessage, sizeof(receiveMessage));
            printf("Get \'%s\' from server.\n", receiveMessage);
        }
    }

    return 0;
}

void split(char *input, char split_word[3][64])
{
    int i = 0, j = 0, k = 0;
    // remove the end of line '\n' charactor
    input[strlen(input) - 1] = '\0';
    while (input[i] != '\0')
    {
        if (input[i] != ' ')
        {
            split_word[j][k] = input[i];
            k++;
        }
        else
        {
            k = 0;
            j++;
        }
        i++;
    }
}
