
#ifndef NETWORK_MAIN_H_
#define NETWORK_MAIN_H_

#include "Message.h"
#include "ThreadSafeQueue.h"

// std
#include <cstdint>
#include <memory>
#include <thread>
#include <vector>
#include <memory>
#include <unordered_map>

// boost
#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>

namespace net {

class Connection;

class NetWork 
	: private boost::noncopyable
{
	friend class Connection;
public:
	NetWork(uint32_t uListenPort, uint16_t uThread);
	~NetWork();

	bool is_runing() { return true; }

	// network start
	void start();
	
	// read a message from net work
	bool read_msg(uint32_t& uSessionID, std::shared_ptr<MsgBase>& spMsg);
	
	// write a message to network
	void write_msg(uint32_t uSessionID, std::shared_ptr<MsgBase> spMsg);

private:
	// private struct: task
	struct Task
	{
		Task() :uSessionID(0), spMsg(nullptr) {}
		Task(uint32_t session_id, std::shared_ptr<MsgBase> sp_msg) :
			uSessionID(session_id), spMsg(sp_msg) {}

		uint32_t uSessionID;
		std::shared_ptr<MsgBase> spMsg;
	};

	// start an asynchronous accept socket
	void _async_accept();

	// called by Connect, push a message into queue  
	void push_into_task_queue(uint32_t uSessionID, std::shared_ptr<MsgBase> spMsg)
	{ taskQueue_.push({ uSessionID, spMsg }); }

private:
	boost::asio::io_service boostIOS_;
	boost::asio::ip::tcp::acceptor boostAcceptor_;
	
	// recv msg queue
	ThreadSafeQueue<Task> taskQueue_;
	
	// threads pool
	std::vector<std::thread*> threads_;

	// connection map
	std::unordered_map<uint32_t, Connection*> sessionsMap_;
};

} // namespace net

#endif