#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100 // �ִ� ���� ��
#define MAX_CLNT 256 // �ִ� ���� ���� ���� ��

void* handle_clnt(void* arg);
void send_msg(char* msg, int len);
void error_handling(char* msg);

// ������ Ŭ���̾�Ʈ ��
int clnt_cnt = 0;
// ���� ���� Ŭ���̾�Ʈ�� �����ϹǷ�, Ŭ���̾�Ʈ ������ �迭�̴�.
// ��Ƽ������ ��, �� �� ��������, clnt_cnt, clnt_socks �� ���� �����尡 ���� ������ �� �ֱ⿡ 
// �� ������ ����� ���ٸ� ������ �Ӱ迵���̴�.
int clnt_socks[MAX_CLNT]; // Ŭ���̾�Ʈ �ִ� 256��
pthread_mutex_t mutx; // mutex ���� - ���� �����峢�� �������� ���� �������� ȥ�� ����

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id; // thread ����

    // ���� �ɼ� ������ ���� �� ����
    int option;
    socklen_t optlen;

    if (argc != 2) // ���ϸ� & port��ȣ
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // ���ؽ� �����
    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    // Time-wait ����
    // SO_REUSEADDR �� 0���� 1�� ����
    optlen = sizeof(option);
    option = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&option, optlen);

    // IPv4 , IP, Port �Ҵ�
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    // �ּ� �Ҵ�
    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    // 5 �� ���������� 5������� ��� ������ ä���ΰ�?
    // �ƴϴ�. ť�� ũ���� ���ε�, �ü���� ������ �ȴٸ� "�˾Ƽ�" accept �� ��
    // ��, �� 256����� ���� ������ ��.
    // ���������� ��õ���� Ŭ���̾�Ʈ�� �ٻ� ���, 15�� ��� ��찡 ������
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    // �������� ����. ctrl + c �θ� ����
    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

        // clnt_socks[], clnt_cnt ���������� ����ϱ� ���� ���ؽ� ���
        pthread_mutex_lock(&mutx);
        // Ŭ���̾�Ʈ ī���� �ø���, ���� ���� . ù ��° Ŭ���̾�Ʈ���, clnt_socks[0] �� �� ��
        clnt_socks[clnt_cnt++] = clnt_sock;
        // ���ؽ� �������
        pthread_mutex_unlock(&mutx);
        // ������ ����. �������� main �� handle_clnt
        // �� ��° �Ķ���ͷ� accept ���� ���ܳ� ������ ���� ��ũ���� �ּҰ��� �־��־�
        // handle_clnt ���� �Ķ���ͷ� ���� �� �ֵ��� ��
        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        // �̰� ȣ���ߴٰ� �ؼ� �������� ���� �����尡 ������� ����
        // ��, t_id �� �������� ��, �ش� �����尡 NULL ���� ������ ��찡 �ƴ϶�� �����ϰ� �����
        // ���� �ش� �����尡 NULL ���� �����ߴٸ�, ������ ����
        pthread_detach(t_id);
        // inet_ntoa �� int32 ������ �� IP �� ĳ���ͷ� ģ���ϰ� �����ִ� ����
        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
    }
    // ctrl + c �� ���α׷� ���� ��, ���� ���� ����
    close(serv_sock);
    return 0;
}

void* handle_clnt(void* arg)
{
    // ���� ���� ��ũ���Ͱ� void �����ͷ� �������Ƿ�, int �� ����ȯ
    int clnt_sock = *((int*)arg);
    int str_len = 0;
    char msg[BUF_SIZE];

    // Ŭ���̾�Ʈ���� ���� �޼��� ����.
    // Ŭ���̾�Ʈ���� EOF ������, str_len �� 0�� �ɶ����� �ݺ�
    // EOF �� ������ ������ �����ΰ�? Ŭ���̾�Ʈ����, ������ close ���� ���̴�!
    // ��, Ŭ���̾�Ʈ�� ������ �ϰ� �ִ� ���ȿ�, while ���� ����� �ʴ´�.
    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
        // ������ ��ο��� �޼��� ������
        send_msg(msg, str_len);

    // while �� Ż���ߴٴ� ��, ���� ����ϴ� ������ ������ �������ٴ� ����.
    // �׷��� �翬��, clnt_socks[] ���� �����ϰ�, �����嵵 �Ҹ���Ѿ�.

    // �������� clnt_cnt �� clnt_socks[] �� �ǵ帱 ���̱⿡, ���ؽ� ���
    pthread_mutex_lock(&mutx);
    // ���� ������ Ŭ���̾�Ʈ�� "���� �����忡�� ����ϴ� ����" ����
    for (int i = 0; i < clnt_cnt; i++)
    {
        // ���� ����ϴ� Ŭ���̾�Ʈ ������ ���� ��ũ���� ��ġ�� ã����,
        if (clnt_sock == clnt_socks[i])
        {
            // ���� ������ ���� ��ġ�ߴ� ���� ��������
            // ���� Ŭ���̾�Ʈ���� ���ܿ�
            while (i++ < clnt_cnt - 1) // ������ 1�� ������ ���̱� ������ -1 ����� ��
                clnt_socks[i] = clnt_socks[i + 1];
            break;
        }
    }
    // Ŭ���̾�Ʈ �� �ϳ� ����
    clnt_cnt--;
    // ���ؽ� �������
    pthread_mutex_unlock(&mutx);
    // ������ �����忡�� ����ϴ� Ŭ���̾�Ʈ ���� ����
    close(clnt_sock);
    return NULL;
}

// ������ ��ο��� �޼��� ������
void send_msg(char* msg, int len)
{
    // clnt_cnt, clnt_socks[] ��� ���� ���ؽ� ���
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++)
        // ��ο��� �޼��� ����
        write(clnt_socks[i], msg, len);
    // ���ؽ� ��� ����
    pthread_mutex_unlock(&mutx);
}
void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}