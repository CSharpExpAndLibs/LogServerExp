/*
 * 指定した周期で指定したバイト数を送信する
 */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

//#define DBG(a) printf a
#define DBG(a)
#define BUFSIZE (4096)


int main(int argc, char* argv[]) {
    int sock; //local socket descripter
    struct sockaddr_in servSockAddr; //server internet socket address
    unsigned short servPort; //server port number
    char sendBuffer[BUFSIZE] = {0}; //send temporary buffer
    char recvBuffer[BUFSIZE] = {0}; //receive temporary buffer
    int bytesPerTrans = 0;
    int wakeUpPeriod = 0;
    
    if (argc != 5) {
        fprintf(stderr, "Usage:%s <IP><Port><Bytes><Period(msec)>\n", argv[0]);
        return EXIT_FAILURE;
    }
    bytesPerTrans = atoi(argv[3]);
    wakeUpPeriod = atoi(argv[4]);
    
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

    // 乱数系列の初期化
    struct timeval tm0;
    gettimeofday(&tm0, NULL);
    srand(tm0.tv_sec);

    while (1) {
        // 送信バッファを乱数で埋める
        for (int j = 0; j < bytesPerTrans; j++) {
            sendBuffer[j] = (char)(rand() % 256);
        }
        if (send(sock, sendBuffer, bytesPerTrans, 0) < 0) {
            perror("send() failed.");
            return EXIT_FAILURE;
        }
        if (recv(sock, recvBuffer, sizeof(recvBuffer), 0) < 0) {
            perror("send() failed.");
            return EXIT_FAILURE;
        }
        DBG(("Send %d Bytes\n", bytesPerTrans));
        usleep(1000*wakeUpPeriod);
    }
    close(sock);

    return EXIT_SUCCESS;
}
