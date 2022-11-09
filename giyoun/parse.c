void parse_uri(char *uri,char *hostname, char *path, int *port) {
  /*
   uri가  
   / , /cgi-bin/adder 이렇게 들어올 수도 있고,
   http://11.22.33.44:5001/home.html 이렇게 들어올 수도 있다.
   알맞게 파싱해서 hostname, port로, path 나누어주어야 한다!
  */

  *port = 80;

  printf("uri=%s\n", uri);
  
  char *parsed;
  parsed = strstr(uri, "//");
  
  if (parsed == NULL) {
    parsed = uri;
  }
  else {
    parsed = parsed + 2;  // 포인터 두칸 이동 
  }
  char *parsed2 = strstr(parsed, ":");

  if (parsed2 == NULL) {
    // ':' 이후가 없다면, port가 없음
    parsed2 = strstr(parsed, "/");
    if (parsed2 == NULL) {
      sscanf(parsed,"%s",hostname);
    } 
    else {
        printf("parsed=%s parsed2=%s\n", parsed, parsed2);
        *parsed2 = '\0';
        sscanf(parsed,"%s",hostname);
        *parsed2 = '/';
        sscanf(parsed2,"%s",path);
    }
  } else {
      // ':' 이후가 있으므로 port가 있음
      *parsed2 = '\0';
      sscanf(parsed, "%s", hostname);
      sscanf(parsed2+1, "%d%s", port, path);
  }
  printf("hostname=%s port=%d path=%s\n", hostname, *port, path);
}


int parse_uri(char* uri, char* filename, char* cgiargs)
{
    char *ptr;

    if (!strstr(uri, "cgi-bin")) { /* Static content*/ // 만일 요청이 정적컨첸츠를 위한 것이라면
        strcpy(cgiargs, "");     // cgi 인자 스트링을 지운다.
        strcpy(filename, ".");      // ./index.html 같은 상대 리눅스 경로이름으로 변환한다.
        strcat(filename, uri);        
        if (uri[strlen(uri)-1] == '/')      // 만일 URI가/ / 문자로 끝난다면
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