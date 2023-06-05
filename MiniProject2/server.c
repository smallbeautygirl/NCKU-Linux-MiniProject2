#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> //Internet Protocol family, ex: sin_family, sin_port, ..., ect
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
/*require (1.socket() 2.bind() 3.listen() 4.accept() 5.read())build up a TCP server */
/*因為pthread規定它裡面的自己的function只能傳一個argument，所以先把要傳的變數包成一個structer*/
struct run_argument
{
    struct sockaddr_in run_cli;
    int run_fd;
};

void *run(void *all);
void split(char *input, char split_word[3][64]);

#define SIZE 512 // buffer size

int main(int argc, char **argv)
{
    int fd;                 /* socket descriptor returned by socket()*/
    int newfd;              /* returned by accept() */
    struct sockaddr_in srv; // used by bind(), use sockaddr_in to store IPv4 address
    struct sockaddr_in cli; // used by accept(),  use sockaddr_in to store IPv4 address
    // struct ifreq ifr;//used by get server ip
    char buf[SIZE];            // used by read()
    int cli_len = sizeof(cli); // used by accept()
    int nbytes;                // used by read()
    int server_port = atoi(argv[1]);
    if (argc != 2)
    {
        fprintf(stderr, "[error] usage: ./server [port]\n");
        exit(1);
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    /*socket procedure*/
    if (fd < 0)
    {
        // AF_INET是指用IPv4 , SOCK_STREAM select the TCP protocol
        perror("socket error");
        exit(1); // 非正常運行檔至退出程序
    }

    /*bind procedure*/
    /* create the socket */
    srv.sin_family = AF_INET;          /* use the Internet address family */
    srv.sin_port = htons(server_port); /* bind socket ‘fd’ to server port */
    /* bind: a client may connect to any of my addresses */
    srv.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(fd, (struct sockaddr *)&srv, sizeof(srv)) < 0)
    {
        perror("bind error");
        exit(1);
    }

    /*listen procedure*/
    if (listen(fd, 5) > 0)
    { //"5" specifies the maximum number of connections that kernel should queue for this socket .
        perror("listen error");
        exit(1);
    }
    while (1)
    {
        /*accept procedure*/
        newfd = accept(fd, (struct sockaddr *)&cli, &cli_len);
        if (newfd < 0)
        {
            perror("accept error");
            exit(1);
        }
        /* create a thread to handle client requst */
        struct run_argument argument = {.run_cli = cli, .run_fd = newfd};

        pthread_t thread; // client thread
        pthread_create(&thread, NULL, run, &argument);
    }

    return 0;
}

void *run(void *all)
{
    struct run_argument run_arg = *(struct run_argument *)all;
    struct sockaddr_in cli = run_arg.run_cli;
    char *cli_ip = inet_ntoa(cli.sin_addr); // 將 network address 由 struct in_addr 轉換為句號與數字組成的字串格式
    int cli_port = ntohs(cli.sin_port);

    printf("A client \"%s\" has connected via port num %d using SOCK_STREAM(TCP)\n", cli_ip, cli_port);

    while (1)
    {

        char buf[64] = {'\0'};
        char split_word[3][64] = {'\0'};
        int result = 0; // for 計算結果
        char resultMessage[64] = {'\0'};

        read(run_arg.run_fd, &buf, sizeof(buf));
        printf("Get: %s\n", buf);
        split(buf, split_word);

        printf("The client \"%s\" with port %d send \'%s\'\n", cli_ip, cli_port, buf);
        if (strcmp(split_word[0], "kill") == 0)
        {
            printf("==kill==\n");
            close(run_arg.run_fd);
            exit(0);
            return NULL;
        }
        else if (strcmp(split_word[0], "add") == 0)
        {
            printf("==add==\n");

            result = atoi(split_word[1]) + atoi(split_word[2]);
            printf("Calculate %d + %d equals to %d\n", atoi(split_word[1]), atoi(split_word[2]), result);

            // 將 result 轉為 string 傳給 client
            sprintf(resultMessage, "%d", result);
            send(run_arg.run_fd, resultMessage, sizeof(resultMessage), 0);
            printf("==add end==\n");
        }
        else if (strcmp(split_word[0], "abs") == 0)
        {
            printf("==abs==\n");

            result = abs(atoi(split_word[1]));
            printf("Calculate |%d| is %d\n", atoi(split_word[1]), result);

            // 將 result 轉為 string 傳給 client
            sprintf(resultMessage, "%d", result);
            write(run_arg.run_fd, resultMessage, sizeof(resultMessage));
            printf("==abs end==\n");
        }
        else if (strcmp(split_word[0], "mul") == 0)
        {
            printf("==mul==\n");

            result = atoi(split_word[1]) * atoi(split_word[2]);
            printf("Calculate %d * %d equals to %d\n", atoi(split_word[1]), atoi(split_word[2]), result);

            // 將 result 轉為 string 傳給 client
            sprintf(resultMessage, "%d", result);
            send(run_arg.run_fd, resultMessage, sizeof(resultMessage), 0);
            printf("==mul end==\n");
        }
        else
        {
            printf("==hello==\n");
            write(run_arg.run_fd, "Hello", sizeof("Hello"));
            printf("==hello end==\n");
        }
        // printf('\n');
    }
}

void split(char *input, char split_word[3][64])
{
    int i = 0, j = 0, k = 0;

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
