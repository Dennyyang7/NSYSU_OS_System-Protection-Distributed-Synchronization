client: client.cpp
	g++ client.cpp -o client
	./client

server: server.cpp
	g++ -pthread -o server server.cpp
	./server

clean_c:
	rm -f client
clean_s:
	rm -f server

