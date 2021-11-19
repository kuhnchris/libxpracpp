
.PHONY = all clean

CC = g++
LINKERFLAG = -lm -lz -I/usr/include/ -lrencodeplus -lpthread
BINS = xpra-c
SFILES = $(SRCS:%.cpp=%.s)

all: ${SFILES} ${BINS}

%.s: %.cpp
	@echo "compiling..."
	${CC} -S -g $<

xpra-c: ${SRCS}
	@echo "Checking.."
	${CC} ${LINKERFLAG} ${SFILES} xpra-c.cpp -g -o $@

clean:
	rm ${BINS} ${SFILES} 2>&1 >/dev/null || echo "1"

