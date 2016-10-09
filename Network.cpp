#include "NetWork.h"
#include "Connection.h"

using namespace net;

NetWork::NetWork(uint32_t uListenPort, uint16_t uThread)
	: boostIOS_()
	, boostAcceptor_(boostIOS_, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string("0.0.0.0"), uListenPort))
	, taskQueue_()
	, threads_(uThread)
	, sessionsMap_()
{
	_async_accept();
}

NetWork::~NetWork()
{
	for (::std::vector<::std::thread>::size_type i = 0; i < threads_.size(); ++i)
		delete threads_[i];;
}

void NetWork::start()
{
	for (std::vector<std::thread>::size_type i = 0; i < threads_.size(); ++i)
	{
		threads_[i] = new std::thread(
			[this]()
		{
			this->boostIOS_.run();
		});
	}
}

void NetWork::_async_accept()
{
	static uint32_t session_id = 0;
	Connection* pConnect = new Connection(this, ++session_id);
	boostAcceptor_.async_accept(pConnect->boostSocket_,
		[this, pConnect](const boost::system::error_code& error)
		{
			pConnect->async_recv();
			this->sessionsMap_.insert(std::make_pair(session_id, pConnect));
			this->_async_accept();
		});
}

bool NetWork::read_msg(uint32_t& uSessionID, std::shared_ptr<MsgBase>& spMsg)
{
	if (taskQueue_.empty())
		return false;
	Task t = taskQueue_.front();
	uSessionID = t.uSessionID;
	spMsg = t.spMsg;
	taskQueue_.pop();
	return true;
}

void NetWork::write_msg(uint32_t uSessionID, std::shared_ptr<MsgBase> spMsg)
{
	std::unordered_map<uint32_t, Connection*>::iterator it =
		sessionsMap_.find(uSessionID);
	if (it == sessionsMap_.end())
		return;
	Connection* pConnect = it->second;
	pConnect->async_send(spMsg);
}