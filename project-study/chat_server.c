#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100 // 최대 글자 수
#define MAX_CLNT 256 // 최대 동시 접속 가능 수

void* handle_clnt(void* arg);
void send_msg(char* msg, int len);
void error_handling(char* msg);

// 접속한 클라이언트 수
int clnt_cnt = 0;
// 여러 명의 클라이언트가 접속하므로, 클라이언트 소켓은 배열이다.
// 멀티쓰레드 시, 이 두 전역변수, clnt_cnt, clnt_socks 에 여러 쓰레드가 동시 접근할 수 있기에 
// 두 변수의 사용이 들어간다면 무조건 임계영역이다.
int clnt_socks[MAX_CLNT]; // 클라이언트 최대 256명
pthread_mutex_t mutx; // mutex 선언 - 다중 스레드끼리 전역변수 사용시 데이터의 혼선 방지

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id; // thread 선언

    // 소켓 옵션 설정을 위한 두 변수
    int option;
    socklen_t optlen;

    if (argc != 2) // 파일명 & port번호
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // 뮤텍스 만들기
    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    // Time-wait 해제
    // SO_REUSEADDR 를 0에서 1로 변경
    optlen = sizeof(option);
    option = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&option, optlen);

    // IPv4 , IP, Port 할당
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    // 주소 할당
    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    // 5 로 지정했으니 5명까지만 사용 가능한 채팅인가?
    // 아니다. 큐의 크기일 뿐인데, 운영체제가 여유가 된다면 "알아서" accept 할 것
    // 즉, 총 256명까지 접속 가능한 것.
    // 웹서버같이 수천명의 클라이언트로 바쁠 경우, 15로 잡는 경우가 보통임
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    // 종료조건 없음. ctrl + c 로만 종료
    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

        // clnt_socks[], clnt_cnt 전역변수를 사용하기 위해 뮤텍스 잠금
        pthread_mutex_lock(&mutx);
        // 클라이언트 카운터 올리고, 소켓 배정 . 첫 번째 클라이언트라면, clnt_socks[0] 에 들어갈 것
        clnt_socks[clnt_cnt++] = clnt_sock;
        // 뮤텍스 잠금해제
        pthread_mutex_unlock(&mutx);
        // 쓰레드 생성. 쓰레드의 main 은 handle_clnt
        // 네 번째 파라미터로 accept 이후 생겨난 소켓의 파일 디스크립터 주소값을 넣어주어
        // handle_clnt 에서 파라미터로 받을 수 있도록 함
        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        // 이걸 호출했다고 해서 끝나지도 않은 쓰레드가 종료되진 않음
        // 즉, t_id 로 접근했을 때, 해당 쓰레드가 NULL 값을 리턴한 경우가 아니라면 무시하고 진행됨
        // 만약 해당 쓰레드가 NULL 값을 리턴했다면, 쓰레드 종료
        pthread_detach(t_id);
        // inet_ntoa 는 int32 형으로 된 IP 를 캐릭터로 친절하게 보여주는 역할
        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
    }
    // ctrl + c 로 프로그램 종료 시, 서버 소켓 종료
    close(serv_sock);
    return 0;
}

void* handle_clnt(void* arg)
{
    // 소켓 파일 디스크립터가 void 포인터로 들어왔으므로, int 로 형변환
    int clnt_sock = *((int*)arg);
    int str_len = 0;
    char msg[BUF_SIZE];

    // 클라이언트에서 보낸 메세지 받음.
    // 클라이언트에서 EOF 보내서, str_len 이 0이 될때까지 반복
    // EOF 를 보내는 순간은 언제인가? 클라이언트에서, 소켓을 close 했을 때이다!
    // 즉, 클라이언트가 접속을 하고 있는 동안에, while 문을 벗어나진 않는다.
    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
        // 접속한 모두에게 메세지 보내기
        send_msg(msg, str_len);

    // while 문 탈출했다는 건, 현재 담당하는 소켓의 연결이 끊어졌다는 뜻임.
    // 그러면 당연히, clnt_socks[] 에서 삭제하고, 쓰레드도 소멸시켜야.

    // 전역변수 clnt_cnt 와 clnt_socks[] 를 건드릴 것이기에, 뮤텍스 잠금
    pthread_mutex_lock(&mutx);
    // 연결 끊어진 클라이언트인 "현재 쓰레드에서 담당하는 소켓" 삭제
    for (int i = 0; i < clnt_cnt; i++)
    {
        // 현재 담당하는 클라이언트 소켓의 파일 디스크립터 위치를 찾으면,
        if (clnt_sock == clnt_socks[i])
        {
            // 현재 소켓이 원래 위치했던 곳을 기준으로
            // 뒤의 클라이언트들을 땡겨옴
            while (i++ < clnt_cnt - 1) // 쓰레드 1개 삭제할 것이기 때문에 -1 해줘야 함
                clnt_socks[i] = clnt_socks[i + 1];
            break;
        }
    }
    // 클라이언트 수 하나 줄임
    clnt_cnt--;
    // 뮤텍스 잠금해제
    pthread_mutex_unlock(&mutx);
    // 서버의 쓰레드에서 담당하는 클라이언트 소켓 종료
    close(clnt_sock);
    return NULL;
}

// 접속한 모두에게 메세지 보내기
void send_msg(char* msg, int len)
{
    // clnt_cnt, clnt_socks[] 사용 위해 뮤텍스 잠금
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++)
        // 모두에게 메세지 보냄
        write(clnt_socks[i], msg, len);
    // 뮤텍스 잠금 해제
    pthread_mutex_unlock(&mutx);
}
void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}