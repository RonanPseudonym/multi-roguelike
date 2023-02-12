#include "../simulator/simulator.hpp"
#include "../network_commons/packet.hpp"
#include <thread>
#include <cstdint>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <queue>
#include <fcntl.h>

// much of this networking code is gratefully and shamelessly stolen from https://beej.us/guide/bgnet/html

namespace Server {
  class Networker {
    public:
  	  int sockfd, rv;
  	  struct addrinfo hints, *servinfo, *p;
  	  std::queue<char*> message_queue;
  	  bool quit_flag = false;

  	  void Init(char* hostname, char* port);
  	  void Listen();

  	  void Send(NetworkCommons::Packet::Packet* packet, sockaddr* to, uint32_t tick);

  	  ~Networker();
  };

  class Server {
    public:
			Networker networker;
			Simulator::Simulator simulator;

			std::thread listener_loop;

	  	Server();
      void Launch(char* hostname, char* port);
      void ListenerLoop(char* hostname, char* port);
      void SimulatorLoop();

      ~Server();
	};
}
