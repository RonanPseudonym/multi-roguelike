#include "snapshot.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <cstring>
#include <inttypes.h>
#include <cstdio>
#include <cstdlib>

namespace NetworkCommons {
	namespace Packet {
		// struct PacketFlags {

		// };

		// struct UnprocessedPacket {
		// 	sockaddr* addr;
		// 	char* msg;
		// };

		// enum values here are used BOTH as systems for encoding and systems for determining type, chart below shows the ranges ascribing to each type

		enum PacketType {
			// === NO DATA === (<= 20)

			// bidirectional

			TEST = 1,

			// server to client

			HANDSHAKE_ALLOW,
			HANDSHAKE_DENY,

			// === TEXT === (<= 40)

			// bidirectional
			TEXT_TEST = 20,

			// === SNAPSHOT === (<= 60)

			// bidirectional
			SNAPSHOT_TEST = 40,

			// === SNAPSHOT === (<= 80)

			// bidirectional
			NUMBER_TEST = 60,

			// === SOCKADDR === (<= 80)

			// client to server
			HANDSHAKE_REQUEST = 80,
		};

		enum DataType {
			NONE, // < 20
			TEXT, // < 40
			SNAPSHOT, // < 60
			NUMBER, // < 80
			SOCKET // < 100
		};

		/*
			PACKET ENCODING: 

			=== header ===
			packet type:   1 byte
			local tick:    4 bytes, unsigned. uint32_t
			packet length: 4 bytes, unsigned. uint32_t
			=== body ===
			data {
				text: plaintext
				snapshot: compressed snapshot serialization (handled in snapshot class)
				number: binary representation (int32_t)
				sockaddr: number + plaintext
			}
		*/


		class Packet {
		    public:
				PacketType packet_type;
				DataType data_type; // type of data
				uint32_t local_tick = 0; // tick that packet was serialized on
				uint32_t num_bytes; // size of packet, shows up when serialized

				union {
					char* text;
					Snapshot::Snapshot* snapshot;
					int32_t* number;
					sockaddr* socket;
				} data;

				Packet(PacketType type);
				Packet(PacketType type, char* _data);
				Packet(PacketType type, int32_t _data);
				Packet(PacketType type, Snapshot::Snapshot* _data);
				Packet(PacketType type, sockaddr* _data);

				Packet(unsigned char* raw);

				uint32_t       DataSerializedSize();
				unsigned char* Serialize(uint32_t tick);

				void Decode(unsigned char* raw);

				void PrintData();
				void DecodeData(unsigned char* raw, uint32_t size);
				void DataTypeFromPacketType();
				void PrettyPrint();

				~Packet();
		};
	}
}