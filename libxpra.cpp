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

using namespace std::string_literals; // enables s-suffix for std::string
                                      // literals

namespace libxpra {
    namespace functions {
        int packetNumber = 0;

        std::string generateFilename(const char* postfix){
            // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
            time_t     now = time(0);
            struct tm  tstruct;
            char       buf[80];
            tstruct = *localtime(&now);
            // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
            // for more information about date/time format
            strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S_", &tstruct);
            strcat(buf, std::to_string(packetNumber).c_str());
            strcat(buf, postfix);
            packetNumber++;
            return buf;

        }
        
        boost::any decodePacket(unsigned char *packetContent,
                                unsigned long bufferSize) {
            boost::any outObject;
            unsigned char *unbuf;
            unbuf = (unsigned char *)malloc(bufferSize * 1024);
            unsigned long unbuf_size = bufferSize * 1024;
            unsigned long buf_size = bufferSize;
            writeToFile(generateFilename(".cap"), string(packetContent, bufferSize));
            if (packetContent[0] == 'P') {
                std::cout << "Package has header char\n";
                int zcompressOK = uncompress(unbuf, &unbuf_size,
                                              &packetContent[8], buf_size-8);
                printf("received (unpacked): ( %lu ) / (%lu) %s\n", unbuf_size,
                       bufferSize, unbuf);
                if (zcompressOK != Z_OK) {
                    printf("Uncompress failed: %s\n", zError(zcompressOK));
                    if (bufferSize >= 8) {
                        string sIn = string(&packetContent[8], bufferSize);
                        if (!rencodeplus::decode::decode(sIn, outObject)) {
                            return boost::any();
                        } else {
                            return outObject;
                        }

                    } else {
                        std::cout << "not a header packet: " << packetContent
                                  << "\n";
                    }
                } else {
                    string sIn = string(unbuf, unbuf_size);
                    writeToFile(generateFilename(".ungziped.cap"), sIn);
                    rencodeplus::decode::decode(sIn, outObject);
                }
            } else {
                std::cout << "not a header packet: " << packetContent
                            << "\n";
                return string(packetContent);
            }
            return outObject;
        }

        void sendHellobyUsingBinaryCapture(int sockfd) {
            int readSize;
            unsigned char *strbindata = rencodeplus::utils::readBinaryData(
                "../data/xpra-orig.bin", readSize);
            send(sockfd, strbindata, readSize, 0);
            printf("sent pre-recorded payload ( %d )...\n", readSize);
        }

        void sendHello(int sockfd) {

            unsigned char buf[2048] = {0};
            buf[0] = 80; // P
            buf[1] = 1;  // rencode
            buf[2] = 1;  // zlib
            buf[3] = 0;  // idx
            buf[4] = 0;  // 4 bytes len
            buf[5] = 0;
            buf[6] = 0;
            buf[7] = 8;

            // unsigned char buf2[1024] = {0};
            unsigned char buf2[2048] = {0};
            uLong buf2size = 2048;

            std::list<boost::any> l;
            string buf3;
            std::map<std::string, boost::any> dict;
            dict.insert(
                std::pair<std::string, boost::any>("compression_level"s, 0));
            dict.insert(std::pair<std::string, boost::any>(
                "xkbmap_keycodes"s, std::list<boost::any>()));
            dict.insert(std::pair<std::string, boost::any>(
                "xkbmap_mod_pointermissing"s, std::list<boost::any>()));
            dict.insert(
                std::pair<std::string, boost::any>("xkbmap_print"s, ""s));
            dict.insert(
                std::pair<std::string, boost::any>("xkbmap_query"s, ""s));
            dict.insert(
                std::pair<std::string, boost::any>("xkbmap_raw"s, false));
            dict.insert(
                std::pair<std::string, boost::any>("xkbmap_variant"s, ""s));
            dict.insert(std::pair<std::string, boost::any>(
                "xkbmap_variants"s, std::list<boost::any>()));

            dict.insert(std::pair<std::string, boost::any>(
                "compressors"s, std::list<boost::any>{"zlib"s, "none"s}));
            dict.insert(std::pair<std::string, boost::any>(
                "encoders"s, std::list<boost::any>{"rencode"s, "bencode"s,
                                                   "yaml"s, "none"s}));
            dict.insert(std::pair<std::string, boost::any>("rencode"s, true));
            dict.insert(std::pair<std::string, boost::any>(
                "rencode.version"s, std::list<boost::any>{"Cython"s, 1, 0, 6}));
            /*dict.insert(std::pair<std::string, boost::any>("version_request"s,
            true)); dict.insert(std::pair<std::string,
            boost::any>("full-version-request"s, true));*/

            dict.insert(std::pair<std::string, boost::any>("windows"s, true));

            dict.insert(std::pair<std::string, boost::any>("none"s, true));
            dict.insert(std::pair<std::string, boost::any>("none.version"s, 0));

            dict.insert(
                std::pair<std::string, boost::any>("version"s, "4.4.4"s));
            dict.insert(
                std::pair<std::string, boost::any>("hostname", "libxpracpp"s));
            dict.insert(std::pair<std::string, boost::any>("uuid", "user"s));
            dict.insert(std::pair<std::string, boost::any>("session-id", "1"s));
            dict.insert(
                std::pair<std::string, boost::any>("username", "service"s));
            dict.insert(std::pair<std::string, boost::any>("user", "service"s));
            dict.insert(
                std::pair<std::string, boost::any>("xdg-menu-update", true));
            dict.insert(
                std::pair<std::string, boost::any>("wants_events", true));
            dict.insert(
                std::pair<std::string, boost::any>("setting-change", true));
            dict.insert(std::pair<std::string, boost::any>(
                "websocket.multi-packet", true));
            dict.insert(
                std::pair<std::string, boost::any>("randr_notify", true));
            dict.insert(std::pair<std::string, boost::any>("mouse", true));
            dict.insert(std::pair<std::string, boost::any>("flush", true));

            std::map<std::string, boost::any> dict_aliases;
            /*dict_aliases.insert(std::pair<std::string, boost::any>("hello",
            1)); dict_aliases.insert(std::pair<std::string,
            boost::any>("disconnect", 20));
            dict_aliases.insert(std::pair<std::string, boost::any>("encodings",
            60));*/
            dict_aliases.insert(
                std::pair<std::string, boost::any>("ack-file-chunk", 7));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("add_data", 14));
            dict_aliases.insert(std::pair<std::string, boost::any>("bell", 41));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("cleanup", 13));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("clipboard-contents", 50));
            dict_aliases.insert(std::pair<std::string, boost::any>(
                "clipboard-contents-none", 51));
            dict_aliases.insert(std::pair<std::string, boost::any>(
                "clipboard-enable-selections", 53));
            dict_aliases.insert(std::pair<std::string, boost::any>(
                "clipboard-pending-requests", 52));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("clipboard-request", 49));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("clipboard-token", 48));
            dict_aliases.insert(std::pair<std::string, boost::any>(
                "configure-override-redirect", 35));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("connection-lost", 22));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("control", 59));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("cursor", 40));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("desktop_size", 26));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("disconnect", 20));
            dict_aliases.insert(std::pair<std::string, boost::any>("draw", 38));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("encodings", 60));
            dict_aliases.insert(std::pair<std::string, boost::any>("eos", 39));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("gibberish", 23));
            dict_aliases.insert(std::pair<std::string, boost::any>("hello", 1));
            dict_aliases.insert(std::pair<std::string, boost::any>("info", 2));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("info-response", 18));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("initiate-moveresize", 31));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("invalid", 24));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("lost-window", 36));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("new-buffer", 12));
            dict_aliases.insert(std::pair<std::string, boost::any>(
                "new-override-redirect", 28));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("new-stream", 10));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("new-tray", 29));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("new-window", 27));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("notify_close", 55));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("notify_show", 54));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("open-url", 3));
            dict_aliases.insert(std::pair<std::string, boost::any>("ping", 16));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("ping_echo", 17));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("pointer-grab", 43));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("pointer-position", 42));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("pointer-ungrab", 44));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("raise-window", 30));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("rpc-reply", 56));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("send-data-request", 5));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("send-data-response", 6));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("send-file", 4));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("send-file-chunk", 8));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("server-event", 19));
            dict_aliases.insert(std::pair<std::string, boost::any>(
                "set-clipboard-enabled", 47));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("set_deflate", 21));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("setting-change", 58));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("show-desktop", 25));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("sound-data", 9));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("startup-complete", 57));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("state-changed", 11));
            dict_aliases.insert(std::pair<std::string, boost::any>("stop", 15));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("udp-control", 61));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("webcam-ack", 46));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("webcam-stop", 45));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("window-icon", 37));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("window-metadata", 34));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("window-move-resize", 32));
            dict_aliases.insert(
                std::pair<std::string, boost::any>("window-resized", 33));

            dict.insert(
                std::pair<std::string, boost::any>("aliases", dict_aliases));
            /*dict.insert(std::pair<std::string, boost::any>("digest",
            std::list<boost::any>{"xor"})); dict.insert(std::pair<std::string,
            boost::any>("salt-digest", std::list<boost::any>{"hmac+sha512"}));*/
            l.push_back("hello"s);
            l.push_back(dict);

            rencodeplus::encode::encode(l, buf3);
            // printf("encoded: %s",buf3.c_str());
            printf("rencoded: ( %lu bytes )...\n", buf3.size());

            int zcompressOK =
                compress2(buf2, &buf2size, buf3.c_str(), buf3.size(), 0);
            if (zcompressOK != Z_OK) {
                printf("Compression failed: %s\n", zError(zcompressOK));
                return;
            }
            int32_t bufSizeSend = buf2size + 1;
            // buf[7] = buf2size + 1;
            buf[7] = 00;
            // memcpy(&buf[4], &bufSizeSend, 4);

            buf[4 + 0] = (bufSizeSend >> 24) & 0xFF;
            buf[4 + 1] = (bufSizeSend >> 16) & 0xFF;
            buf[4 + 2] = (bufSizeSend >> 8) & 0xFF;
            buf[4 + 3] = bufSizeSend & 0xFF;
            memcpy(&buf[8], buf2, buf2size);
            std::cout << "uncompressed..."
                      << "\n";
            rencodeplus::utils::writeToFile("uncompressed", string(buf3));
            std::cout << "compressed..."
                      << "\n";
            rencodeplus::utils::writeToFile("compressed.z",
                                            string(buf2, buf2size));
            std::cout << "packet..."
                      << "\n";
            rencodeplus::utils::writeToFile("packet.z",
                                            string(buf, bufSizeSend));

            send(sockfd, buf, bufSizeSend + 8, 0);
            printf("sent payload ( %d + 8 )...\n", bufSizeSend);
        }
    } // namespace functions
} // namespace libxpra