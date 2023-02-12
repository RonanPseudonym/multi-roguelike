#include "packet.hpp"

void NetworkCommons::Packet::Packet::DataTypeFromPacketType() {
	uint8_t int_type = ((uint8_t)floor((uint8_t)packet_type / 20));
	data_type = (DataType)int_type;
}

NetworkCommons::Packet::Packet::Packet(PacketType type) {
	packet_type = type;
	DataTypeFromPacketType();
}

NetworkCommons::Packet::Packet::Packet(PacketType type, char* _data) {
	packet_type = type;
	data.text = _data;
	data_type = DataType::TEXT;
}

NetworkCommons::Packet::Packet::Packet(PacketType type, int32_t _data) {
	packet_type = type;
	data.number = (int32_t*)malloc(4);
	*data.number = _data;
	data_type = DataType::NUMBER;
}

NetworkCommons::Packet::Packet::Packet(PacketType type, Snapshot::Snapshot* _data) {
	packet_type = type;
	data.snapshot = _data;
	data_type = DataType::SNAPSHOT;
}

NetworkCommons::Packet::Packet::Packet(PacketType type, sockaddr* _data) {
	packet_type = type;
	data.socket = _data;
	data_type = DataType::SOCKET;
}

void NetworkCommons::Packet::Packet::PrintData() {
	switch (data_type) {
		case DataType::NONE:     printf("[None]"); break;
		case DataType::TEXT:     printf("\"%s\"", data.text); break;
		case DataType::NUMBER:   printf("%d", (int)*data.number); break;
		case DataType::SNAPSHOT: printf("[Snapshot]"); break;
		case DataType::SOCKET:   {
			printf("%hu :: ", data.socket->sa_family);
			for (int i = 0; i < 14; i ++) {
				printf("%d ", data.socket->sa_data[i]);
			}

			break;
		}
	}
}

void NetworkCommons::Packet::Packet::PrettyPrint() {
	printf("Packet {\n");
	printf("  Local Tick:  %" PRIu32 "\n", local_tick);
	printf("  Packet Type: %d\n", (int)packet_type);
	printf("  Data Type:   %d\n", (int)data_type);
	printf("  Data:        ");
	PrintData();
	printf("\n}\n");
}

// unsigned char* NetworkCommons::Packet::Packet::SerializeData(uint32_t data_size) {
// 	switch (data_type) {
// 		case DataType::NONE: {
// 			unsigned char* buf = (unsigned char*)malloc(data_size);
// 			memcpy(buf, "\0", 1); 
// 			return buf;
// 		}
// 		case DataType::TEXT: {
// 			unsigned char* buf = (unsigned char*)malloc(data_size);
// 			memcpy(buf, data.text, strlen(data.text) + 1);
// 			return buf;
// 		}
// 		case DataType::NUMBER: {
// 			unsigned char* buf = (unsigned char*)malloc(data_size);
// 			*buf++ = *data.number>>(int32_t)24; *buf++ = *data.number>>(int32_t)16;
//     		*buf++ = *data.number>>(int32_t)8;  *buf++ = *data.number;

//     				printf("a\n");

//     		return buf;
// 		}
// 		case DataType::SNAPSHOT: return data.snapshot->Serialize();
// 	}
// }

uint32_t NetworkCommons::Packet::Packet::DataSerializedSize() {
	switch (data_type) {
		case DataType::NONE:     return 1;
		case DataType::TEXT:     return strlen(data.text) + 1;
		case DataType::NUMBER:   return 4;
		case DataType::SNAPSHOT: return data.snapshot->SerializedSize();
		case DataType::SOCKET:   return 4 + 14;
	}

	return 0;
}

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
		socket: number + text
	}
*/

unsigned char* NetworkCommons::Packet::Packet::Serialize(uint32_t tick) {
	uint32_t data_size = DataSerializedSize();

	num_bytes = 9 /* header size */ + data_size /* need space for \0? */;
	unsigned char* serialization = (unsigned char*)malloc(num_bytes);

	serialization[0] = packet_type;

	serialization[1] = tick >> 24;
	serialization[2] = tick >> 16;
	serialization[3] = tick >> 8;
	serialization[4] = tick;

	serialization[5] = num_bytes >> 24;
	serialization[6] = num_bytes >> 16;
	serialization[7] = num_bytes >> 8;
	serialization[8] = num_bytes;

	switch (data_type) {
		case DataType::NONE: {
			serialization[9] = '\0';
			break;
		}
		case DataType::TEXT: {
			memcpy(serialization + 9, data.text, data_size);
			break;
		}
		case DataType::NUMBER: {
			serialization[9]  = *data.number >> 24;
			serialization[10] = *data.number >> 16;
			serialization[11] = *data.number >> 8;
			serialization[12] = *data.number;

			break;
		}
		case DataType::SNAPSHOT: data.snapshot->Serialize(serialization); break;
		case DataType::SOCKET: {
			serialization[9]  = *data.number >> 24;
			serialization[10] = *data.number >> 16;
			serialization[11] = *data.number >> 8;
			serialization[12] = *data.number;

			memcpy(serialization + 9 + 4, data.socket->sa_data, 14);
			break;
		}
	}

	return serialization;
}

uint32_t BytesToUint32t(unsigned char* bytes, uint32_t offset) {
	return ((uint32_t)bytes[offset]<<24) |
           ((uint32_t)bytes[offset+1]<<16) |
           ((uint32_t)bytes[offset+2]<<8)  |
           bytes[offset+3];
}

void NetworkCommons::Packet::Packet::DecodeData(unsigned char* raw, uint32_t size) {
	
	switch (data_type) {
		case DataType::NONE: {
			break;
		}
		case DataType::TEXT: {
			data.text = (char*)malloc(size);
			for (uint32_t i = 0; i < size; i ++) {
				data.text[i] = raw[i+9];
			}
			break;
		}
		case DataType::NUMBER: {
			int32_t i;
			uint32_t i2 = ((uint32_t)raw[9]<<24) |
                          ((uint32_t)raw[10]<<16) |
                          ((uint32_t)raw[11]<<8)  |
                           raw[12];

		    // change unsigned numbers to signed
		    if (i2 <= 0x7fffffffu) { i = i2; }
		    else { i = -1 - (int32_t)(0xffffffffu - i2); }

		    data.number = (int32_t*)malloc(4);
		    *data.number = i;

			break;
		}
		case DataType::SNAPSHOT: data.snapshot = new Snapshot::Snapshot(raw); break;
		case DataType::SOCKET:   {
			data.socket = (sockaddr*)malloc(sizeof(unsigned short) + 14);

			data.socket->sa_family = (unsigned short)BytesToUint32t(raw, 9);
			for (uint32_t i = 0; i < size; i ++) {
				data.socket->sa_data[i] = raw[i+9+4];
			}
			break;
		}
	}
}

void NetworkCommons::Packet::Packet::Decode(unsigned char* raw) {
	packet_type = (PacketType)(int)raw[0];
	DataTypeFromPacketType();

	local_tick = BytesToUint32t(raw, 1);
	num_bytes = BytesToUint32t(raw, 5);

	DecodeData(raw, num_bytes - 9);

	free(raw);
}

NetworkCommons::Packet::Packet::Packet(unsigned char* raw) {
	Decode(raw);
}

NetworkCommons::Packet::Packet::~Packet() {
	switch (data_type) {
		// case DataType::TEXT:     free(data.text); break;
		case DataType::SNAPSHOT: delete [] data.snapshot; break;
		case DataType::NUMBER:   free(data.number); break;
		// case DataType::SOCKET:   free(data.socket); break;
	}
}