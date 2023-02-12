CLIENT = src/client.cpp
SERVER = src/server.cpp src/server/server.cpp src/server/networker.cpp
COMMONS = src/network_commons/packet.cpp src/network_commons/snapshot.cpp
FLAGS = -Wall -Wpedantic -Wformat --std=c++11 -ggdb3
INCLUDE = -I lib/boost
CMD = g++ $(COMMONS) $(FLAGS) $(INCLUDE)

client:
	$(CMD) $(CLIENT) -o builds/client

server:
	$(CMD) $(SERVER) -o builds/server

both:
	make server
	make client
