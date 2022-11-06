#  main.c kor.c usa.c
#  --> main.o kor.o usa.o
#  --> binary (app.out)

# Target(만들고자 하는 파일) : Dependency (필요로 하는 파일들)
# 	command (실행 명령)

CC = gcc     #환경변수
TARGET = app.out	#환경변수
OBJS = main.o kor.o usa.o

CFLAGS = -Wall 	#컴파일할때 옵션을 띄울 수 있음 ex)디버거나 오류
LDFLAGS = -lc		#링크할 때 옵션 / c library

all : $(TARGET)  # all 옵션이 없는 경우 제일 첫번째 Target만 실행 시키고 종료한다. / 순서대로 실행

$(TARGET) : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^		
	
# $(CC) -o $(TARGET) $(OBJS)

.c.o :  # .c 파일을 .o로 다 바꾸어 주겠다.
	$(CC) $(CFLAGS) -c -o $@ $<
# /-------------------------------------------
# main.o:
# 	$(CC) -c main.c

# kor.o :
# 	$(CC) -c kor.c

# usa.o :
# 	$(CC) -c usa.c

clean : 
	rm -f $(OBJS) $(TARGET)