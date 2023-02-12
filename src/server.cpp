#include "server/server.hpp"
#include <cstring>

int main() {
	Server::Server server;
	server.Launch("localhost", "12345");

	// NetworkCommons::Packet::Packet p1(NetworkCommons::Packet::PacketType::NUMBER_TEST, -17);
	// unsigned char* serialization = p1.Serialize(123456);
	// p1.PrettyPrint();

	// for (int i = 0; i < p1.DataSerializedSize() + 9; i ++) {
	// 	printf("%c", serialization[i]);
	// }

	// printf("\n");

	// for (int i = 0; i < p1.DataSerializedSize() + 9; i ++) {
	// 	printf("%d ", serialization[i]);
	// }

	// printf("\n");

	// NetworkCommons::Packet::Packet p2(serialization);
	// p2.PrettyPrint();
}