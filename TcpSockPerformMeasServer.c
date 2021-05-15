/*
 * 一つのトランザクション毎に"END"をクライアントに送信する
 */
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#define DBG(a) printf a
#define DBG(a)
#define QUEUELIMIT 5
#define BUFSIZE (4096)

int main(int argc, char* argv[]) {

    int servSock; //server socket descriptor
    int clitSock; //client socket descriptor
    struct sockaddr_in servSockAddr; //server internet socket address
    struct sockaddr_in clitSockAddr; //client internet socket address
    unsigned short servPort; //server port number
    unsigned int clitLen; // client internet socket address length
    char recvBuffer[BUFSIZE];
    char *sendBuffer = "END";
    int byteRcvd;
    int totalBytesRcvd;

    if (argc != 2) {
        fprintf(stderr, "Usage:%s <port>\n", argv[1]);
        return EXIT_FAILURE;
    }

    if ((servPort = (unsigned short) atoi(argv[1])) == 0) {
        fprintf(stderr, "invalid port number.\n");
        return EXIT_FAILURE;
    }

    if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ){
        perror("socket() failed.");
        return EXIT_FAILURE;
    }

    memset(&servSockAddr, 0, sizeof(servSockAddr));
    servSockAddr.sin_family      = AF_INET;
    servSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servSockAddr.sin_port        = htons(servPort);

    if (bind(servSock, (struct sockaddr *) &servSockAddr,
             sizeof(servSockAddr) ) < 0 ) {
        perror("bind() failed.");
        return EXIT_FAILURE;
    }

    if (listen(servSock, QUEUELIMIT) < 0) {
        perror("listen() failed.");
        return EXIT_FAILURE;
    }

    while (1) {
        clitLen = sizeof(clitSockAddr);
        if ((clitSock = accept(servSock, (struct sockaddr *) &clitSockAddr,
                               &clitLen)) < 0) {
            perror("accept() failed.");
            return EXIT_FAILURE;
        }
        printf("connected from %s.\n", inet_ntoa(clitSockAddr.sin_addr));

        // クライアントから明示的に接続切断されるまで受信を続ける
        totalBytesRcvd = 0;
        while (1) {
            byteRcvd = recv(clitSock, recvBuffer, sizeof(recvBuffer), 0);
            if(byteRcvd == 0){
                printf("Conneciton Closed\n");
                break;
            } else if (byteRcvd < 0){
                perror("recv() failed.");
                break;
            }
            if (send(clitSock, sendBuffer, strlen(sendBuffer)+1, 0) < 0) {
                perror("send() failed.");
                return EXIT_FAILURE;
            }
            totalBytesRcvd += byteRcvd;
        }
        close(clitSock);
        printf("total Bytes received=%d\n", totalBytesRcvd);
    }

    return EXIT_SUCCESS;
}
