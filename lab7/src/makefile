CC=gcc

tcp-server:
	$(CC) tcpserver.c -o tcpserver

tcp-client:
	$(CC) tcpclient.c -o tcpclient

udp-server:
	$(CC) udpserver.c -o udpserver

udp-client:
	$(CC) udpclient.c -o udpclient

all: tcp-server tcp-client udp-server udp-client

clean:
	rm -rf tcpclient tcpserver udpclient udpserver &> /dev/null