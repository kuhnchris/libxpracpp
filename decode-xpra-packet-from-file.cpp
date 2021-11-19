#include <boost/any.hpp>
#include <cstddef>
#include <fstream>
#include <ios>
#include <iostream>
#include <librencodeplus.hpp>
#include <list>
#include <map>
#include <string>
#include <zconf.h>
#include <zlib.h>

int decode_file(std::string filename, bool forceDecompress) {
    string ostr;
    boost::any outObjStrBin;
    int readSize;
    unsigned char *strbindata =
        rencodeplus::utils::readBinaryData(filename, readSize);
    std::cout << "Reading and decoding file... " << filename
              << " (size: " << readSize << ")"
              << "\n";
    if (strbindata != nullptr) {
        if (strbindata[0] == 'P') {
            std::cout << "P found - header. stripping the header."
                      << "\n";
            strbindata = &strbindata[8];
        }
        if (forceDecompress) {
            std::cout << "force uncompressing"
                      << "\n";
            unsigned char *uncompr;
            unsigned long rSize = readSize;
            unsigned long tSize = readSize * 2;
            uncompr = (unsigned char *)malloc(tSize * sizeof(char));
            int zUncompressError = uncompress2(uncompr, &tSize, strbindata, &rSize);
            if (zUncompressError != Z_OK) {
                printf("Compression failed: %s\n", zError(zUncompressError));
                return -1;
            } else {
                std::cout << "Uncompressing done." << "\n";
            }
            strbindata = uncompr;
            readSize = tSize;
        }
        string strBin(strbindata, readSize);
        rencodeplus::decode::decode(strBin, outObjStrBin);
        std::cout << "Decoding done, printing...\n";
        rencodeplus::utils::outputDataStructure(outObjStrBin, 0);
        std::cout << "\n";
    } else {
        return 1;
    }
    return 0;
}

bool findArgv(int argc, char **argv, const char *option) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], option) == 0) {
            return true;
        }
    }
    return false;
}
char* GetArgvWithout(int argc, char **argv, const char *option) {
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], option, strlen(option)) == 0) {
            return argv[i];
        }
    }
    return nullptr;
}

void printUsage(char **argv) {

    std::cout << "Decode tool for librencodeplus."
              << "\n";
    std::cout << "usage: " << argv[0] << " [filename]"
              << "\n\n";
    std::cout << "Options:"
              << "\n";
    std::cout << "--help | -h - help (this)"
              << "\n";
    std::cout << "-z          - force uncompress (after payload header)"
              << "\n";
}

int main(int argc, char **argv) {
    if (argc > 1) {
        bool forceDecompress = false;
        if (findArgv(argc, argv, "-z")) {
            forceDecompress = true;
        }
        if (findArgv(argc, argv, "-h") || findArgv(argc, argv, "--help")) {
            printUsage(argv);
            return 0;
        } else {
            decode_file(GetArgvWithout(argc,argv,"-"), forceDecompress);
        }
    } else {
        printUsage(argv);
        return 0;
    }
}