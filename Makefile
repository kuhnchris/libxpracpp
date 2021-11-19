
.PHONY = all clean

CC = g++
LINKERFLAG = -lm -lz -I/usr/include/ -lrencodeplus -lpthread
BINS = xpra-c decode-xpra-packet-from-file
SFILES = $(SRCS:%.cpp=%.s)

all: ${SFILES} ${BINS}

%.s: %.cpp
	@echo "compiling..."
	${CC} -S -g $<

xpra-c: ${SRCS}
	@echo "Checking.."
	${CC} ${LINKERFLAG} ${SFILES} xpra-c.cpp -g -o $@

decode-xpra-packet-from-file: ${SRCS}
	@echo "Checking.."
	${CC} ${LINKERFLAG} ${SFILES} decode-xpra-packet-from-file.cpp -g -o $@

clean:
	rm ${BINS} ${SFILES} 2>&1 >/dev/null || echo "1"

