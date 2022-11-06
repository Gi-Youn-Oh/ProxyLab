#include <string.h>
#include <stdio.h>

int main()
{
	char dest[] = "abcdefgabcd";
    char *first;
    char *last;

	/* dest 문자열에서 첫번째 나오는 'b'문자를 찾습니다. */
	first = strchr(dest, 'b');

	/* dest 문자열에서 마지막 나오는 'b'문자를 찾습니다. */
	last = strrchr(dest, 'b');

	printf("first : %s, last : %s\n", first, last);
    
	return 0;
}

// 결과값은 first : bcdefgabcd, last : bcd 입니다.
