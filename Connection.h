
#ifndef NETWORK_CONNECTION_H_
#define NETWORK_CONNECTION_H_

#include "ThreadSafeQueue.h"

// std
#include <memory>

// boost
#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>

namespace net {

class MsgBase;
class NetWork;

class Connection 
	: private boost::noncopyable
{
	friend class NetWork;
public:
	Connection(NetWork* pNet, uint32_t uSessionID);
	~Connection();

public:
	// start an asynchronous recv msg
	void async_recv();

	// start an asynchronous send msg
	void async_send(std::shared_ptr<MsgBase> spMsgBase);

	// shutdown the tcp connection
	void shutdown();

private:
	// asynchronous recv msg funtions
	void start_async_recv();
	void async_recv_head();
	void async_recv_body();

	// asynchronous send msg funtions
	void start_async_send(std::shared_ptr<MsgBase> spMsgBase);
	void async_send_head();
	void async_send_body();

private:
	NetWork* pNet;
	uint32_t uSessionID_;
	boost::asio::ip::tcp::socket boostSocket_;
	std::shared_ptr<MsgBase> spRecvMsg;
	std::shared_ptr<MsgBase> spSendMsg;
	ThreadSafeQueue<std::shared_ptr<MsgBase>> sendMsgQueue_;
};

} // namespace net

#endif