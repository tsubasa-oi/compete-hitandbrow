#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

#define PORT_NUMBER 12345
#define BUFFER_SIZE 1024

struct ClientData {
    int socket;
};

// 4桁の数値をグローバル変数化する
int firstNum;
int secondNum;
int thirdNum;
int forthNum;

// クライアント番号を定義
// int clientCount = 0;

// ユーザーの入力を許可するかどうかを示すフラグ
// int isInputAllowed = 1;
// 排他制御 
pthread_mutex_t mutex;

void numeron(){
    srand((unsigned)time(NULL));
    firstNum = rand() % 10;

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
}

void* handle_client(void* arg);

int main() {
    // ソケットの設定
    int server_socket;
    struct sockaddr_in server_address;

    // サーバーソケットの作成
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // プロトコル設定
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NUMBER);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // ソケットオプションを設定してソケットの再利用を有効にする
    // int enable = 1;
    // if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    // perror("setsockopt(SO_REUSEADDR) failed");
    // exit(EXIT_FAILURE);
    // }

    // バインド
    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    // リッスン　2つのリクエストまで許可
    listen(server_socket, 2); 
    printf("接続を待機中...\n");

    numeron();

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Mutex 初期化失敗");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // クライアントからの接続待ち
        int client_socket = accept(server_socket, NULL, NULL);
        printf("新規のクライアントからの接続を受付しました。\n");

        // クライアントの数を増やす
        // clientCount++;

        // スレッドを使ってクライアントの処理を開始
        pthread_t thread_id;
        struct ClientData* client_data = (struct ClientData*)malloc(sizeof(struct ClientData));
        client_data->socket = client_socket;

        if (pthread_create(&thread_id, NULL, handle_client, (void*)client_data) != 0) {
            perror("スレッド作成に失敗");
            exit(EXIT_FAILURE);
        }

        // スレッドの終了を待たない（ノンブロッキング）
        pthread_detach(thread_id);
    }

    // mutexの解放
    pthread_mutex_destroy(&mutex);

    // サーバーソケットのクローズ
    close(server_socket);

    return 0;
}

void* handle_client(void* arg) {
    struct ClientData* client_data = (struct ClientData*)arg;
    int client_socket = client_data->socket;
    free(client_data);

    int arraySize = 4;
    int total;
    int inputNum;
    int inputArray[4];
    int hit;
    int brow;
    int count = 0;
    // defineで定義した1024
    char buffer[BUFFER_SIZE];

    do {
        // クライアント側からデータを受信
        if (recv(client_socket, buffer, BUFFER_SIZE, 0) == -1) {
            perror("Error receiving data");
            exit(EXIT_FAILURE);
        }

        // mutexをロック
        printf("ミューテックスをロックします。\n");
        pthread_mutex_lock(&mutex);

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

        // mutexをアンロック
        pthread_mutex_unlock(&mutex);
        printf("ミューテックスをアンロックします。\n");

    } while (hit != 4);  // クライアントが正解を送るまで繰り返す

    // 通信が終了したらクライアントソケットをクローズ
    close(client_socket);

    pthread_exit(NULL);
}
