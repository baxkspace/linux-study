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

// ä��â�� ������ �̸��� ����
char name[NAME_SIZE] = "[DEFAULT]"; // ���� �г��� 20�� ����
char msg[BUF_SIZE];

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    // �۽� ������� ���� ������� �� 2���� ������ ����
    // �� �޼����� �������ϰ�, ������ �޼����� �޾ƾ� �Ѵ�.
    pthread_t snd_thread, rcv_thread;
    // pthread_join �� ���ȴ�.
    void* thread_return;
    // �̹��� ip, port �Ӹ� �ƴ϶�, ����� �̸����� �־���� �Ѵ�.
    if (argc != 4)
    {
        printf("Usage : %s <IP> <port> <name>\n", argv[0]);
        exit(1);
    }

    // argv[3] �� Jony ���, "[Jony]" �� name �� ��
    sprintf(name, "[%s]", argv[3]);

    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    // �� ���� ������ �����ϰ�, ������ main �� send_msg, recv_meg
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);

    // ������ ���� ��� �� �Ҹ� ����
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);

    // Ŭ���̾�Ʈ ���� ����
    close(sock);
    return 0;
}

// snd_thread �� ������ main
void* send_msg(void* arg)
{
    // void�� int������ ��ȯ
    int sock = *((int*)arg);
    // ����� ���̵�� �޼����� "�ٿ���" �� ���� ���� ���̴�
    char name_msg[NAME_SIZE + BUF_SIZE];
    while (1)
    {
        // �Է¹���
        fgets(msg, BUF_SIZE, stdin);
        // Q �Է� �� ����
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            // ������ EOF �� ����
            close(sock);
            exit(0);
        }
        // id �� "Jony", msg �� "�ȳ� ����" �� �ߴٸ�, => [Jony] �ȳ� ����
        // �̰��� name_msg �� ���� ��µ�
        sprintf(name_msg, "%s %s", name, msg);
        // ������ �޼��� ����
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}

// rcv_thread �� ������ main
void* recv_msg(void* arg)
{
    int sock = *((int*)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    int str_len;
    while (1)
    {
        // �������� ���� �޼��� ����
        str_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1);
        // str_len �� -1 �̶�� ��, ���� ���ϰ� ������ �������ٴ� ����
        // �� �������°�? send_msg ���� close(sock) �� ����ǰ�,
        // ������ EOF �� ������, ������ �װ� �޾Ƽ� "�ڱⰡ ����" Ŭ���̾�Ʈ ������ close �� ��
        // �׷��� read ���� �� ����� -1 �� ��.
        if (str_len == -1)
            // ���Ḧ ���� ���ϰ�. thread_return ���� �� ��
            return (void*)-1; // pthread_join�� �����Ű�� ����

          // ���� �� ������ �� NULL
        name_msg[str_len] = 0;
        // ���� �޼��� ���
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