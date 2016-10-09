
#include "Network.h"
#include <iostream>
#include <Windows.h>

int main(int ac, char** av)
{
	net::NetWork network(7999, 4);
	network.start(); 
	
	uint32_t sessionID;
	std::shared_ptr<net::MsgBase> spRecvMsg;
	std::shared_ptr<net::MsgBase> spSendMsg;
	while (network.is_runing())
	{
		while (network.read_msg(sessionID, spRecvMsg))
		{
			std::cout << "Task's Session ID:" << sessionID << std::endl;
			std::cout << "Task's Type:" << spRecvMsg->uType_ << std::endl;
			std::cout << "Task's Size:" << spRecvMsg->uSize_ << std::endl;
			std::cout.write(spRecvMsg->pContent_, spRecvMsg->uSize_);

			network.write_msg(sessionID, spRecvMsg);
		}
	}
	return 0;
}