CC = g++
CFLAGS = -Wall -pthread

all: server client1 client2

server: server.cpp
	$(CC) $(CFLAGS) -o server server.cpp

client1: client1.cpp
	$(CC) $(CFLAGS) -o client1 client1.cpp CPluginEnumerator1.cpp CPluginController.cpp IPrintPlugin.cpp -ldl
	
client2: client2.cpp
	$(CC) $(CFLAGS) -o client2 client2.cpp CPluginEnumerator2.cpp CPluginController.cpp IPrintPlugin.cpp -ldl

clean:
	rm -f server client1 client2

