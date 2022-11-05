#include "csapp.h"

int main(int argc, char **argv)
{
    int clientfd;
    char *host, *port, buf[MAXLINE];
    rio_t rio;
    
    if (argc !=3){
        fprintf(stderr, "usage: %s <host><port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = argv[2];

    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    while (Fgets(buf, MAXLINE, stdin) != NULL){  // EOF 표준입력을 만날때 까지
        Rio_writen(clientfd, buf, strlen(buf)); //서버에 텍스트줄을 전송
        Rio_readlineb(&rio, buf, MAXLINE);  // 서버에서 echo줄을 read
        Fputs(buf, stdout); // 결과 표준 출력 인쇄
    }
    Close(clientfd); // 식별자 닫기 / 서버로 EOF 통지 전송 / 클라이언트의 커널이 프로세스가 종료할때 자동으로 열었던 모든 식별자를 닫아줌 명시적 표시
    exit(0);
}