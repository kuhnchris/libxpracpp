#include "./libxpra.h"
#include <arpa/inet.h> //inetaddr
#include <boost/any.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <librencodeplus.hpp>
#include <list>
#include <map>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h> //socket/open/...
#include <thread>
#include <zconf.h>
#include <zlib.h>

bool request_quit = false;

const int packetSize = 512 * 1024;

void _thread_loop(int sockfd) {
    bool sent_hello = false;
    unsigned char *fullBuf;
    unsigned long fullBufLen = 0;
    while (!request_quit) {
        if (!sent_hello) {
            // sendHello(sockfd);
            libxpra::functions::sendHellobyUsingBinaryCapture(sockfd);
            sent_hello = true;
        }

        unsigned char buf[packetSize] = {0};
        long bufread = recv(sockfd, &buf, packetSize, 0);

        printf("new packet start!\n");
        while (bufread > 0) {
            printf("received (packet): ( %lu )\n", bufread);
            unsigned char *newBuf;
            newBuf = (unsigned char *)malloc(fullBufLen + bufread);
            memcpy(newBuf, fullBuf, fullBufLen);
            memcpy(newBuf + fullBufLen, buf, bufread);
            fullBuf = newBuf;
            fullBufLen = fullBufLen + bufread;
            printf("total yet received (packet): ( %lu )\n", fullBufLen);

            bufread = recv(sockfd, &buf, packetSize, MSG_DONTWAIT);
        }
        printf("total received (packet): ( %lu )\n", fullBufLen);
        if (fullBufLen > 0) {
            /*            rencodeplus::utils::writeToFile("hello-reply.bin",
                                                        string(packetContent,
               bufferSize));*/
            boost::any outObject =
                libxpra::functions::decodePacket(fullBuf, fullBufLen);
            if (!outObject.empty()) {
                rencodeplus::utils::outputDataStructure(outObject, 0);
                std::cout << "\n";
                printf("clearing buffer!\n");
                /*fullBufLen = 0;
                fullBuf = NULL;*/
            } else {
              boost::any outObject;
              string b = string(fullBuf[9],fullBufLen-9);
              rencodeplus::decode::decode(b, outObject);
              rencodeplus::utils::outputDataStructure(outObject, 0);
              std::cout << "\n";
              /*if (!outObject.empty()){

                rencodeplus::utils::outputDataStructure(outObject, 0);
                std::cout << "\n";
                fullBufLen = 0;
                fullBuf = NULL;
              }*/
            }
        }
        /*if (bufread == -1) {
            std::cout
                << "an error occoured. socket is closed. closing connection.\n";
            request_quit = true;
        }*/
        sleep(1);
    }
}

int main() {
    int sockfd;
    struct sockaddr_in remote;

    printf("starting with zlib: %s\n", zlibVersion());
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("socket went fucky.\n");
        return -1;
    }
    remote.sin_addr.s_addr = inet_addr("127.0.0.1");
    remote.sin_port = htons(10000);
    remote.sin_family = AF_INET;
    if (connect(sockfd, (struct sockaddr *)&remote, sizeof(remote)) < 0) {
        printf("connect failed");
        return -2;
    }

    std::thread t(_thread_loop, sockfd);

    while (!request_quit) {
        char q;
        std::cout << "press 'q' to quit.";
        std::cin >> q;
        if (q == 'q') {
            request_quit = true;
            t.join();
        }
    }
    return 0;
}