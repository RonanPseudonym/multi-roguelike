#include "server.hpp"
#include <iostream>
#include <ctime>
#include <chrono>

// TODO: look into multicasting
#define SIMULATED_FPS 20

Server::Server::Server() {
}

void Server::Server::ListenerLoop(char* hostname, char* port) {
	networker.Init(hostname,port);
	networker.Listen(); // loops until quit_flag == true
}

void Server::Server::Launch(char* hostname, char* port) {
  listener_loop = std::thread(&Server::Server::ListenerLoop, this, hostname, port); // thread 1
  
  std::chrono::steady_clock::time_point start, end;
  uint32_t cycle_count = 0; // tick counter

  for (;;) { // thread 2
  	cycle_count ++;
	  start = std::chrono::steady_clock::now();
 
	  while (!networker.message_queue.empty()) { // loop through message queue and process each message
	  	std::cout << "Tick " << cycle_count << " :: " << networker.message_queue.front() << std::endl;
	  	free(networker.message_queue.front());
	  	networker.message_queue.pop();
	  }

	  end = std::chrono::steady_clock::now();
	  std::this_thread::sleep_for(std::chrono::milliseconds(1000/SIMULATED_FPS - std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count())); // wait long enough to balance out time to SIMULATED_FPS
  }

  networker.quit_flag = true; // gracefully end thread 2
  listener_loop.join();
}

Server::Server::~Server() {

}