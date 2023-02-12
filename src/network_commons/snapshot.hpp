#include <inttypes.h>

namespace NetworkCommons {
	namespace Snapshot {
		class Snapshot {
			public:
				Snapshot();
				Snapshot(unsigned char* raw);
				void Serialize(unsigned char* serialization);
				uint32_t SerializedSize();
				~Snapshot();
		};
	}
}