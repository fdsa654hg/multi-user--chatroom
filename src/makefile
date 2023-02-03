all:server client

server:s_main.cpp handler.cpp Server.cpp
	g++ s_main.cpp handler.cpp Server.cpp -o server -lpthread

client:c_main.cpp Client.cpp ClientState.cpp ConsoleUI.cpp UI.cpp
	g++ c_main.cpp Client.cpp ClientState.cpp ConsoleUI.cpp UI.cpp -o client -lpthread

clean:
	rm -f client server