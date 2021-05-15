CC = gcc
CFLAGS = -g3

TARGETS = TcpSockServer TcpSockClient DomainSockServer \
TcpSockPerformMeasServer TcpSockPerformMeasClient \
TcpSockIterClient

all: $(TARGETS)

TcpSockServer: TcpSockServer.o
	$(CC) $(CFLAGS) -o $@ $^

TcpSockClient: TcpSockClient.o
	$(CC) $(CFLAGS) -o $@ $^

TcpSockPerformMeasServer: TcpSockPerformMeasServer.o
	$(CC) $(CFLAGS) -o $@ $^

TcpSockPerformMeasClient: TcpSockPerformMeasClient.o
	$(CC) $(CFLAGS) -o $@ $^

TcpSockIterClient: TcpSockIterClient.o
	$(CC) $(CFLAGS) -o $@ $^

DomainSockServer: DomainSockServer.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *~ *.o $(TARGETS)
