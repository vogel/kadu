
#ifndef PENDING_MSGS_H
#define PENDING_MSGS_H

#include <qvaluelist.h>
#include "misc.h"
#include "../libgadu/lib/libgadu.h"

class PendingMsgs
{
	public:
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
		int count();
		Element &operator[](int index);
		void addMsg(UinsList uins, QString msg, int msgclass, time_t time);
		bool loadFromFile();
};

#endif
