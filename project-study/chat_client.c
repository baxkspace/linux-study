#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(char* msg);

// 채팅창에 보여질 이름의 형태
char name[NAME_SIZE] = "[DEFAULT]"; // 본인 닉네임 20자 제한
char msg[BUF_SIZE];

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    // 송신 쓰레드와 수신 쓰레드로 총 2개의 쓰레드 선언
    // 내 메세지를 보내야하고, 상대방의 메세지도 받아야 한다.
    pthread_t snd_thread, rcv_thread;
    // pthread_join 에 사용된다.
    void* thread_return;
    // 이번엔 ip, port 뿐만 아니라, 사용자 이름까지 넣어줘야 한다.
    if (argc != 4)
    {
        printf("Usage : %s <IP> <port> <name>\n", argv[0]);
        exit(1);
    }

    // argv[3] 이 Jony 라면, "[Jony]" 가 name 이 됨
    sprintf(name, "[%s]", argv[3]);

    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    // 두 개의 쓰레드 생성하고, 각각의 main 은 send_msg, recv_meg
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);

    // 쓰레드 종료 대기 및 소멸 유도
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);

    // 클라이언트 연결 종료
    close(sock);
    return 0;
}

// snd_thread 의 쓰레드 main
void* send_msg(void* arg)
{
    // void형 int형으로 전환
    int sock = *((int*)arg);
    // 사용자 아이디와 메세지를 "붙여서" 한 번에 보낼 것이다
    char name_msg[NAME_SIZE + BUF_SIZE];
    while (1)
    {
        // 입력받음
        fgets(msg, BUF_SIZE, stdin);
        // Q 입력 시 종료
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            // 서버에 EOF 를 보냄
            close(sock);
            exit(0);
        }
        // id 를 "Jony", msg 를 "안녕 얘들아" 로 했다면, => [Jony] 안녕 얘들아
        // 이것이 name_msg 로 들어가서 출력됨
        sprintf(name_msg, "%s %s", name, msg);
        // 서버로 메세지 보냄
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}

// rcv_thread 의 쓰레드 main
void* recv_msg(void* arg)
{
    int sock = *((int*)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    int str_len;
    while (1)
    {
        // 서버에서 들어온 메세지 수신
        str_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1);
        // str_len 이 -1 이라는 건, 서버 소켓과 연결이 끊어졌다는 뜻임
        // 왜 끊어졌는가? send_msg 에서 close(sock) 이 실행되고,
        // 서버로 EOF 가 갔으면, 서버는 그걸 받아서 "자기가 가진" 클라이언트 소켓을 close 할 것
        // 그러면 read 했을 때 결과가 -1 일 것.
        if (str_len == -1)
            // 종료를 위한 리턴값. thread_return 으로 갈 것
            return (void*)-1; // pthread_join를 실행시키기 위해

          // 버퍼 맨 마지막 값 NULL
        name_msg[str_len] = 0;
        // 받은 메세지 출력
        fputs(name_msg, stdout);
    }
    return NULL;
}

void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}