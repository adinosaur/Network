
#ifndef NETWORK_MESSAGE_H_
#define NETWORK_MESSAGE_H_

#include <cstdint>

namespace net {

class MsgBase
{
public:
	MsgBase() = default;
	~MsgBase() { if (pContent_) delete pContent_; }

	uint16_t uType_;
	uint16_t uSize_;
	char* pContent_;
};

#define MSGBASE_MAX_SIZE	INT16_MAX - 1
#define MSGBASE_HEAD_SIZE	4

} // namespace net

#endif