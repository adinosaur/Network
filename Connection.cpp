#include "Connection.h"
#include "Message.h"
#include "NetWork.h"

using namespace net;

Connection::Connection(NetWork* pNet, uint32_t uSessionID)
	: pNet(pNet)
	, uSessionID_(uSessionID)
	, boostSocket_(pNet->boostIOS_)
	, spRecvMsg(nullptr)
	, spSendMsg(nullptr)
	, sendMsgQueue_()
{
}

Connection::~Connection()
{
}

void Connection::async_recv()
{
	start_async_recv();
}

void Connection::start_async_recv()
{
	spRecvMsg = std::shared_ptr<MsgBase>(new MsgBase);
	async_recv_head();
}

void Connection::async_recv_head()
{
	boost::asio::async_read(boostSocket_,
		boost::asio::buffer(spRecvMsg.get(), MSGBASE_HEAD_SIZE),
		[this](const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		if (error)
		{
			this->shutdown();
			return;
		}

		if (this->spRecvMsg->uSize_ > MSGBASE_MAX_SIZE)
		{
			this->shutdown();
			return;
		}

		this->spRecvMsg->pContent_ = new char[this->spRecvMsg->uSize_];
		this->async_recv_body();
	});
}

void Connection::async_recv_body()
{
	boost::asio::async_read(boostSocket_,
		boost::asio::buffer(spRecvMsg->pContent_, spRecvMsg->uSize_),
		[this](const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		if (error)
		{
			this->shutdown();
			return;
		}

		pNet->push_into_task_queue(this->uSessionID_, this->spRecvMsg);
		this->start_async_recv();
	});
}

void Connection::async_send(std::shared_ptr<MsgBase> spMsgBase)
{
	if (spSendMsg != nullptr)
	{
		sendMsgQueue_.push(spMsgBase);
		return;
	}
	start_async_send(spMsgBase);
}

void Connection::start_async_send(std::shared_ptr<MsgBase> spMsgBase)
{
	spSendMsg = spMsgBase;
	async_send_head();
}

void Connection::async_send_head()
{
	boost::asio::async_write(boostSocket_,
		boost::asio::buffer(spSendMsg.get(), MSGBASE_HEAD_SIZE),
		[this](const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		if (error)
		{
			this->shutdown();
			return;
		}
		this->async_send_body();
	});
}

void Connection::async_send_body()
{
	boost::asio::async_write(boostSocket_,
		boost::asio::buffer(spSendMsg->pContent_, spSendMsg->uSize_),
		[this](const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		if (error)
		{
			this->shutdown();
			return;
		}

		this->spSendMsg = nullptr;

		if (!this->sendMsgQueue_.empty())
		{
			std::shared_ptr<MsgBase> spMsgBase = sendMsgQueue_.front();
			sendMsgQueue_.pop();
			this->start_async_send(spMsgBase);
		}
	});
}

void Connection::shutdown()
{
	try
	{
		boostSocket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		boostSocket_.close();
	}
	catch (std::exception& e)
	{
		e.what();
	}
}