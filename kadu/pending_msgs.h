
#ifndef PENDING_MSGS_H
#define PENDING_MSGS_H

#include <qvaluelist.h>

class PendingMsgs
{
	public:
		struct Element
		{
			QValueList<uin_t> uins;
			QString msg;
			int msgclass;
			time_t time;
		};

	private:
		QValueList<Element> msgs;
		
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
};

#endif
