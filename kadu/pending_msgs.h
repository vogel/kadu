
#ifndef PENDING_MSGS_H
#define PENDING_MSGS_H

#include <qvaluelist.h>
#include "../libgadu/lib/libgadu.h"

class PendingMsgs
{
	public:
		typedef QValueList<uin_t> UinsList;
		struct Element
		{
			UinsList uins;
			QString msg;
			int msgclass;
			time_t time;
		};

	private:
		typedef QValueList<Element> PendingMsgsList;
		PendingMsgsList msgs;
		void saveToFile();
		
	public:
		PendingMsgs();
		void deleteMsg(int index);
		bool pendingMsgs(uin_t uin);
		bool pendingMsgs();
		Element& accessNextMsg(uin_t uin);
		Element& accessNextMsg();
		void deleteNextMsg(uin_t uin);
		void deleteNextMsg();
		void addMsg(uin_t sender,QString msg,int msgclass,time_t time);
		void addUin(uin_t uin);
		bool loadFromFile();
};

#endif
