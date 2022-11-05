#include "csapp.h"

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0){    // EOF 를 만날때까지 반복 텍스트줄을 읽고 써준다.
        printf("server received %d bytes\n", (int)n);       
        Rio_writen(connfd, buf, n);
    }
}