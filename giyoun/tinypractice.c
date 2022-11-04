
/*
* tiny.c - A simple, iterative HTTP/1.0 Web server that uses the Get method to serve static and dynamic content
*/

#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char* uri, char *filename, char* cgiargs);
void serve_static(int fd, char* filename, int filesize);
void get_filetype(char* filename, char *filetype);
void serve_dynamic(int fd, char* filename, char* cgiargs);
void clienterror(int fd, char* cause, char* errnum, char* shortmsg, char* longmsg);

int main(int argc, char** argv)
{
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    /* Check command-line args*/
    if (argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1){
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen); // 반복적으로 연결 요청을 접수하고
        Getnameinfo((SA*)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);
        doit(connfd); // 트랜잭션 수행
        Close(connfd);  // 자신 쪽의 연결 끝을 닫는다.
    }
}

void doit(int fd)
{
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;

    /* Read request line and headers*/
    // 요청라인을 읽고 분석
    Rio_readinitb(&rio, fd);
    Rio_readlineb(&rio, buf, MAXLINE);
    printf("Request headers:\n");
    printf("%s", but);
    sscanf(buf, "%s %s %s", method, uri, version);
    if(strcasecmp(method, "GET")){ // 만일 클라이언트가 GET 이 아닌 다른 메소드를 요청하면 에러메시지를 보내고 main루틴으로 돌아온다.
        clienterror(fd, method, "501", "Not implemented", "Tiny dose not implement this method");
        return;
    }
    read_requesthdrs(&rio); 

    /* Parse URI from GET request*/
    is_static = parse_uri(uri, filename, cgiargs); // uri파일이름과 비어있을수도 있는 CGI인자 스트링을 분석, 정적 동적인지 플래그 설정
    if (stat(filename, &sbuf) < 0){
        clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file"); // 만일 파일이 디스크상에 없으면, 에러메시지 를 보내고 리턴한다.
        return;
    }    

    if (is_static) { /* Serve static content */  // 요청이 정적컨텐츠를 위한 것이라면
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)){ // 파일이 보통파일이라는 것, 읽기 권한을 가지고 있는지 검증
            clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
            return;
        }
        serve_static(fd, filename, sbuf.st_size); // 보통파일이고 읽기권한이 있다면 클아이언트에게 제공
    }
    else{ /* Serve dynamic content */
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)){ // 동적 컨텐츠라면 실행가능한지 검증
            clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI progam");

            return;
        }
        serve_dynamic(fd, filename, cgiargs); // 동적컨텐츠이고 실행가능하면 제공
    }
}

void clienterror(int fd, char* cause, char* errnum, char* shortmsg, char* longmsg)
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}   

