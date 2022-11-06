#include <stdlib.h>

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
    Rio_writen(fd, buf, strlen(buf));
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
// mmap , Munmap --> malloc, rio_readn, free 사용 연결 식별자에게 복사

    srcp = (char*) malloc(sizeof(char) * filesize); // serve_static 함수에서 srcp = char형으로 선언했기 때문에 char형으로 받고,char * filesize 만큼 곱해준다.
    rio_readn(srcfd, srcp, filesize);           //srcfd에서 srcp로 file size를 읽는다.
    
    free (srcp);
