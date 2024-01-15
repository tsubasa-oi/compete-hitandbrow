#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT_NUMBER 12345
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    char buffer[BUFFER_SIZE];
    int guess;

    // クライアントソケットの作成
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // connectしていく
    struct sockaddr_in server_address;
    // IPv4
    server_address.sin_family = AF_INET;
    // アドレス指定
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    // ポート指定
    server_address.sin_port = htons(PORT_NUMBER);
    // サーバーに接続
    connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    printf("接続しました。\n");

    do {
        // 4桁のランダムな数値を入力する
        printf("4桁の数値を入力してください：");
        scanf("%d", &guess);

        // 数値を文字列に変換して送信
        sprintf(buffer, "%d", guess);
        send(client_socket, buffer, strlen(buffer), 0);
        
        // サーバーからの結果を受信
        recv(client_socket, buffer, BUFFER_SIZE, 0);

        // 受信したデータを表示
        printf("%s\n", buffer);

        // 正解なら終了
        if (strcmp(buffer, "正解です。今入力したプレイヤーの勝ちです。") == 0) {
            break;
        }

        // サーバーの入力待ちを待つ
        printf("相手の入力待ち...\n");
        recv(client_socket, buffer, BUFFER_SIZE, 0);

    } while (1);

    // 通信を終了する
    close(client_socket);

    return 0;
}
