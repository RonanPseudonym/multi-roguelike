#include "snapshot.hpp"
#include <cstring>
#include <cstdlib>

NetworkCommons::Snapshot::Snapshot::Snapshot() {

}

NetworkCommons::Snapshot::Snapshot::Snapshot(unsigned char* raw) {
	free(raw);
}

uint32_t NetworkCommons::Snapshot::Snapshot::SerializedSize() {
	return 0;
}

void NetworkCommons::Snapshot::Snapshot::Serialize(unsigned char* serialization) {
}

NetworkCommons::Snapshot::Snapshot::~Snapshot() {

}