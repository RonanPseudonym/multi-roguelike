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

// === ARCHITECTURE ===
// thread 1:
//   game loop, does simulations and message processing. set fps
// thread 2:
//   recieves messages and queues them for thread 1 to process

/* === INITIAL CONNECTION ===
  client -> server: HANDSHAKE_REQUEST, along with client's sockaddr
  server -> client, with recv(): HANDSHAKE_ACCEPT, adds client to list of clients (sockaddr[4])
  client -> server (recvfrom()): READY_TO_BEGIN
  [wait until all clients are ready]
  server: instead of using recv, uses recvfrom looping through list of clients
*/

namespace Server {
  class Networker { // handles incoming and sending messages
    public:
  	  int sockfd, rv;                       // |        |
  	  struct addrinfo hints, *servinfo, *p; // unix stuff
  	  std::queue<NetworkCommons::Packet::UnprocessedPacket*> message_queue; // messages are queued here in thread 2 and then processed in thread 1, the game cycle
  	  bool quit_flag = false; // to merge thread 2 back into thread 1

  	  void Init(char* hostname, char* port); // set up server
  	  void Listen(); // thread 2

  	  void Send(NetworkCommons::Packet::Packet* packet, sockaddr* to, uint32_t tick);

  	  ~Networker();
  };

  class Server {
    public:
			Networker networker;
			Simulator::Simulator simulator; // run in thread 1

			std::thread listener_loop; // thread 2

	  	Server();
      void Launch(char* hostname, char* port); // set up everything, and kick threads into action
      void ListenerLoop(char* hostname, char* port); // thread 2 calls this
      void SimulatorLoop(); // thread 1 calls this

      ~Server();
	};
}
