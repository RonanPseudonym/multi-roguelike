#include "server.hpp"
#include <iostream>

void Server::Networker::Init(char* hostname, char* port) { // set up server
	memset(&hints, 0, sizeof(hints)); // clear hints
	hints.ai_family = AF_INET6; // ipv6
	hints.ai_socktype = SOCK_DGRAM; // udp

	if ((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) { // if unable to get server address from ip and port
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) { // loop through servinfo and make a socket (in this case, servinfo is just addrinfo of hostname:port)
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("networker: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("networker: bind");
			continue;
		}

		break;
	}

	// int yes=1;
	// if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
	//     perror("setsockopt");
	//     exit(1);
	// }

  	// NetworkCommons::Packet::Packet p1(NetworkCommons::Packet::PacketType::HANDSHAKE_REQUEST, p->ai_addr);
  	// p1.PrettyPrint();

  	// NetworkCommons::Packet::Packet p2(p1.Serialize(123));
  	// p2.PrettyPrint();

	fcntl(sockfd, F_SETFL, O_NONBLOCK); // nonblocking -- recv and recvfrom don't wait until they get a message, they just keep on looping

	if (p == NULL) {
		fprintf(stderr, "networker: failed to create socket\n");
		return;
	}
}

void Server::Networker::Listen() { // thread 2
	while (!quit_flag) {
		char* msg = (char*)calloc(256, sizeof(char));
		recv(sockfd, msg, 255, 0);

		if (msg[0]) {
			NetworkCommons::Packet::UnprocessedPacket* up;
			up->data = (unsigned char*)msg;
			message_queue.push(up); // push to end of queue if message recieved
		}
	}
}

void Server::Networker::Send(NetworkCommons::Packet::Packet* packet, sockaddr* to, uint32_t tick) {
	unsigned char* data = packet->Serialize(tick);
	sendto(sockfd, data, packet->num_bytes, 0, to, sizeof(*to));

	free(data);
}

Server::Networker::~Networker() {
	while (!message_queue.empty()) {
		free(message_queue.front());
	  	message_queue.pop();
	}

	freeaddrinfo(servinfo);
	close(sockfd);
}