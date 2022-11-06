/*
 * adder.c - a minimal CGI program that adds two numbers together
 */
/* $begin adder */

/* $end adder */
// 두 인자를 더하고 클라이언트에게 결과와 HTML 파일을 리턴하는 CGI 프로그램

#include "csapp.h"

int main(void){
    char *buf, *p;
    char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
    int n1=0, n2=0;

    /* Extract the two arguments */
    // if ((buf = getenv("QUERY_STRING")) != NULL) {
	//     p = strchr(buf, '&');
	//     *p = '\0';
	//     strcpy(arg1, buf);
	//     strcpy(arg2, p+1);
	//     n1 = atoi(arg1);
	//     n2 = atoi(arg2);
    // }
    
    /* Extract the two arguments */
    if ((buf = getenv("QUERY_STRING")) != NULL){
        p = strchr(buf, '&');
        *p = '\0'; // NULL 이랑 같은 표현
        sscanf(buf, "num1=%d", &n1);
        sscnaf(p+1, "num2=%d", &n2 );
        n1 = atoi(arg1);
        n2 = atoi(arg2);
    }

    /* Make the response body */
    sprintf(content, "QUERY_STRING=%s", buf);
    sprintf(content, "Welcome to add.com: ");
    sprintf(content, "%sTHE Internet addition portal.\r\n<p>", content);
    sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>", content, n1, n2, n1 + n2);
    sprintf(content, "%sThanks for visiting!\r\n", content);

    /* Generate the HTTP response */
    printf("Connection: close\r\n");
    printf("Content-length: %d\r\n", (int)strlen(content));
    printf("Content-type: text/html\r\n\r\n");
    printf("%s", content);
    fflush(stdout);

    exit(0);

}