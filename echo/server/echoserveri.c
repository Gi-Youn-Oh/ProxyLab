
// clientaddr = accept로 보내지는 소켓주소구조체
// accept가 return하기 전에 clientaddr에는 연결의 다른 쪽 끝의 클라이언트 소켓 주소로 채워진다.
// echo 서버는 한번에 한개의 클라이언트만 처리가능 

#include "csapp.h"
#include "echo.c"
void echo(int connfd);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;   // Enough space for any address
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    listenfd = Open_listenfd(argv[1]); // 듣기 식별자 오픈
    while (1) {             // 클라이언트로부터 연결 요청 대기, 도메인 이름과 연결된 클라이언트의 포트를 출력, 클라이언트를 서비스하는 echo함수 호출
        clientlen = sizeof(struct sockaddr_storage);        
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
        echo(connfd);
        Close(connfd);
    }
    exit(0);
}