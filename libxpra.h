#include <boost/any.hpp>

namespace libxpra {
    namespace functions {
        void sendHellobyUsingBinaryCapture(int sockfd);
        void sendHello(int sockfd);
        boost::any decodePacket(unsigned char *packetContent, unsigned long bufferSize);
    } // namespace functions
} // namespace libxpra