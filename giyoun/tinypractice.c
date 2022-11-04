
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

// 요청헤더를 읽고 무시한다.
void read_requesthdrs(rio_t *rp) 
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    while(strcmp(buf, "\r\n")){
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }
    return;
}
// Tiny 는 정적 컨텐츠를 위한 홈 디렉토리가 자신의 현재 디렉토리이고, 실행파일의 홈 디렉토리는 /cgi-bin이라고 가정
// 스트링 cgi-bin 을 포함하는 모든 URI는 동적 컨텐츠를 요청하는 것을 나타낸다고 가정, 기본파일이름 ./home.html
/* URI를 파일 이름과 옵션으로 CGI 인자 스트링을 분석 */
int parse_uri(char* uri, char* filename, char* cgiargs)
{
    char *ptr;

    if (!strstsr(uri, "cgi-bin")) { /* Static content*/ // 만일 요청이 정적컨첸츠를 위한 것이라면
        strcpy(cgiargs, "");     // cgi 인자 스트링을 지운다.
        strcpy(filename, ".");      // ./index.html 같은 상대 리눅스 경로이름으로 변환한다.
        strcat(filename, uri);        
        if (uri[strlen(uri)-1] == '/')      // 만일 URI가 / 문자로 끝난다면
            strcat(filename, "home.html");   // 기본파일이름을 추가한다.
        return 1;
    }
    else{ /* Dynamic content */   // 만일 동적컨텐츠를 위한 것이라면
        ptr = index(uri, '?');   // 모든 cgi 인자들 추출
        if(ptr) {
            strcpy(cgiargs, ptr+1);
            *ptr = '\0';
        }
        else   
            strcpy(cgiargs, "");
        strcpy(filename, ".");   // 나머지 URI 부분을 상대 리눅스 파일 이름으로 변환
        strcat(filename, uri);
        return 0;
    }
}

// Tiny는 5개의 서로 다른 정적 컨텐츠 타입을 지원한다. (HTML, 무형식 text file, GIF, PNG, JPEG으로 인코딩된 영상)
void serve_static(int fd, char* filename, int filesize)
{
    int srcfd;
    char* srcp, filetype[MAXLINE], buf[MAXBUF];

    /* Send response headers to client */
    get_filetype(filename, filetype); // 파일 이름의 접미어 부분을 검사해서 파일 타입을 결정하고
    sprintf(buf, "HTTP/1.0 200 OK\r\n");     // 클라이언트에 응답줄과 응답 헤더를 보낸다.
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf); 
    sprintf(buf, "%sConnection: close\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
    Rio_writen(bf, buf, strlen(buf));
    printf("Response headers:\n");
    printf("%s", buf);

    /* Send response body to client */ // 요청한 파일의 내용을 연결 식별자 fd로 복사해서 응답 본체를 보낸다.
    srcfd = Open(filename, O_RDONLY, 0);      // 읽기 위해서 filename을 open , 식별자를 얻어온다.
    srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);  // mmap 함수는 요청한 파일을 가상메모리 영역으로 매핑한다.
     // mmap을 호출하면 파일 srcfd의 첫번째 filesize 바이트를 주소 srcp에서 시작하는 사적 읽기-허용 가상 메모리 영역으로 매핑한다.
    Close(srcfd); // 파일을 메모리로 매핑한 후에 더 이상 이 식별자는 필요없으며, 그래서 이 파일을 닫는다. (메모리 누수 방지)                              
    Rio_writen(fd, srcp, filesize); // 실제로 파일을 클라이언트에게 전송 주소srcp에서 시작하는 filesize바이트를 클라이언트의 연결 식별자로 복사한다. ( 물론 이것은 요청한 파일에 매핑되어 있다.)           
    Munmap(srcp, filesize); // 매핑된 가상메모리 주소를 반환한다. (메모리 누수 방지)
}

/* get filetype - Derive file type from filename */

void get_filetype(char* filename, char* filetype){
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".git"))
        strcpy(filetype, "image/fit");
    else if (strstr(filename, ".png"))
        strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else   
        strcpy(filetype, "text/plain");
}


// Tiny는 자식 프로세스를 fork(복제) 하고 , 그 후에 CGI 프로그램을 자식의 컨텍스트에서 실행하며 모든 종류의 동적 콘텐츠를 제공한다.
void serve_dynamic(int fd, char* filename, char* cgiargs){
    char buf[MAXLINE], *emptylist[] = {NULL};

    /* Return first part of HTTP response */
    // 응답
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Tiny Web server\r\n");
    Rio_writen(fd, buf, strlen(buf));

    if (Fork() == 0 { /* Child */   // 새로운 자식 프로세스를 fork
        /* Real server would set all CGI vars here */
        setenv("QUERY_STRING", cgiargs, 1);         // 자식은 query string 환경변수를 요청 URI의 CGI 인자들로 초기화한다.
        Dup2(fd, STDOUT_FILENO);  /* Redirect stdout to client */    // 자식은 자식의 표준 출력을 연결 파일 식별자로 재지정
        Execve(filename, emptylist, environ); /* Run CGI program */     // 그 후에 프로그램을 로드하고 실행
    }
    Wait(NULL); /* Parent waits for and reaps child*/       // 부모는 자식이 종료되어 정리되는 것을 기다리기 위해 wait 함수에서 block
}