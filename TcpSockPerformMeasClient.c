/*
 * トランザクションあたりのバイト数とトランザクション数を
 * 指定しサーバーに対してデータを送付する。
 * トランザクション開始からサーバー応答が返ってくるまでの時間を
 * 測定して、全トランザクションでのMax/Min/Aveを計算する。
 */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define DBG(a) printf a
#define BUFSIZE (4096)

void DispValues(int *diffs, int count, int bytes)
{
    // usec order
    int max = diffs[0];
    int min = diffs[0];
    int ave = 0;
    for (int i = 0; i < count; i++) {
        if (max < diffs[i])
            max = diffs[i];
        else if (min > diffs[i])
            min = diffs[i];
        ave += diffs[i];
    }
    ave /= count;

    // 表示
    printf("--- Bytes=%d, Count=%d ---\n", bytes, count);
    printf("Max=%d\n", max);
    printf("Min=%d\n", min);
    printf("Ave=%d\n", ave);
    printf("kbps=%lf\n", 1000 * (double)bytes/(double)ave);
}

int main(int argc, char* argv[]) {
    int sock; //local socket descripter
    struct sockaddr_in servSockAddr; //server internet socket address
    unsigned short servPort; //server port number
    char sendBuffer[BUFSIZE] = {0}; //send temporary buffer
    char recvBuffer[BUFSIZE] = {0}; //receive temporary buffer
    int bytesPerTrans = 0;
    int transactionNum = 0;
    
    if (argc != 5) {
        fprintf(stderr, "Usage:%s <IP><Port><Bytes><Count>\n", argv[0]);
        return EXIT_FAILURE;
    }
    bytesPerTrans = atoi(argv[3]);
    transactionNum = atoi(argv[4]);
    
    memset(&servSockAddr, 0, sizeof(servSockAddr));

    servSockAddr.sin_family = AF_INET;

    if (inet_aton(argv[1], &servSockAddr.sin_addr) == 0) {
        fprintf(stderr, "Invalid IP Address.\n");
        return EXIT_FAILURE;
    }

    if ((servPort = (unsigned short)atoi(argv[2])) == 0) {
        fprintf(stderr, "invalid port number.\n");
        return EXIT_FAILURE;
    }
    servSockAddr.sin_port = htons(servPort);

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ){
        perror("socket() failed.");
        return EXIT_FAILURE;
    }

    if (connect(sock, (struct sockaddr*)&servSockAddr,
                sizeof(servSockAddr)) < 0) {
        perror("connect() failed.");
        return EXIT_FAILURE;
    }

    printf("connect to %s\n", inet_ntoa(servSockAddr.sin_addr));

    // 時間差を格納するバッファ
    int *diff = (int *)malloc(transactionNum * sizeof(int));
    // 時間差を測定するためのデータ
    struct timeval tm0;
    struct timeval tm1;
    // 乱数系列の初期化
    gettimeofday(&tm0, NULL);
    srand(tm0.tv_sec);

    for (int i = 0; i < transactionNum; i++) {
        // 送信バッファを乱数で埋める
        for (int j = 0; j < bytesPerTrans; j++) {
            sendBuffer[j] = (char)(rand() % 256);
        }
        // 送信->受信間の時間を測定
        gettimeofday(&tm0, NULL);
        if (send(sock, sendBuffer, bytesPerTrans, 0) < 0) {
            perror("send() failed.");
            return EXIT_FAILURE;
        }
        if (recv(sock, recvBuffer, sizeof(recvBuffer), 0) < 0) {
            perror("send() failed.");
            return EXIT_FAILURE;
        }
        gettimeofday(&tm1, NULL);
        // 時間差計算 (usec)
        diff[i] = (tm1.tv_sec-tm0.tv_sec)*1000000 + (tm1.tv_usec-tm0.tv_usec);
        DBG(("Count=%d,Time=%d,Recv=%s\n", i, diff[i], recvBuffer));
    }
    // Max/Min/Aveの計算と表示
    DispValues(diff, transactionNum, bytesPerTrans);
    
    close(sock);

    return EXIT_SUCCESS;
}
