#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

#define PORT_NUMBER 12345
#define BUFFER_SIZE 1024

// int countMatchingDigits(int num1, int num2);

int main() {
    // ソケットの設定
    int server_socket, client_socket;
    // アドレスを構造体に格納しちゃう
    struct sockaddr_in server_address, client_address;
    // defineで定義した1024
    char buffer[BUFFER_SIZE];

    // サーバーソケットの作成
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // プロトコル設定
    server_address.sin_family = AF_INET;
    // server_address.sin_addr.s_addr = INADDR_ANY;
    //ポート番号をdefineで定義した12345に指定
    server_address.sin_port = htons(PORT_NUMBER);

    // バインド
    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    // リッスン　2つのリクエストまで許可
    listen(server_socket, 2); 
    printf("接続を待機中...\n");

    // クライアントからの情報を受け入れる
    client_socket = accept(server_socket, (struct sockaddr*)&client_address, (socklen_t*)&client_address);
    printf("Connection accepted from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    //　ここからヌメロンの番号を生成していく
    srand((unsigned)time(NULL));
    
    int firstNum = rand() % 10;
    int secondNum;
    int thirdNum;
    int forthNum;

    // 4桁の重複しない乱数を生成
    do {
        secondNum = rand() % 10;
    } while (firstNum == secondNum);

    do {
        thirdNum = rand() % 10;
    } while (firstNum == thirdNum | secondNum == thirdNum);

    do {
        forthNum = rand() % 10;
    } while (firstNum == forthNum | secondNum == forthNum | thirdNum == forthNum);

    // //確認用
     printf("4つのランダムな整数を出力しました:%d,%d,%d,%d\n", firstNum,secondNum,thirdNum,forthNum);

    int arraySize = 4;
    int total;
    int inputNum;
    int inputArray[4];
    int hit;
    int brow;
    int count = 0;

    do {
        // クライアント側からデータを受信
        recv(client_socket, buffer, BUFFER_SIZE, 0);

        // 受信したデータを整数に変換する
        sscanf(buffer, "%d", &inputNum);

        //それぞれの値を配列に格納
        inputArray[0] = inputNum / 1000;
        inputArray[1] = (inputNum % 1000) / 100;
        inputArray[2] = (inputNum % 100) / 10;
        inputArray[3] = inputNum % 10;

        // hitとbrowの数を0に定義
        hit = 0;
        brow = 0;

        // 的中していればhitの値を追加する
        for (int i = 0 ; i < arraySize ; i++){
            if(inputArray[i] == firstNum){
                hit++;
                break;
            }
        }
        for (int i = 0 ; i < arraySize ; i++){
            if(inputArray[i] == secondNum){
                hit++;
                break;
            }
        }
        for (int i = 0 ; i < arraySize ; i++){
            if(inputArray[i] == thirdNum){
                hit++;
                break;
            }
        }
        for (int i = 0 ; i < arraySize ; i++){
            if(inputArray[i] == forthNum){
                hit++;
                break;
            }
        }

        brow = arraySize - hit;
        count++;

        //　カウント数を明示するために文字列を定義
        char ansmessage[256];
        char failmessage[256];
        sprintf(ansmessage, "正解です。今入力したプレイヤーの勝ちです。");
        sprintf(failmessage, "不正解です。hit:%d brow:%d\n現在%d回目の入力です。", hit,brow,count);

        if (hit == 4) {
            send(client_socket, ansmessage, strlen(ansmessage), 0);
        } else {
            send(client_socket, failmessage, strlen(failmessage), 0);
        }

    } while (hit != 4);  // クライアントが正解を送るまで繰り返す

    // 通信を終了する
    close(server_socket);
    close(client_socket);

    return 0;
}
