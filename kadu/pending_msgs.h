
#ifndef PENDING_MSGS_H
#define PENDING_MSGS_H

#include <qvaluelist.h>
#include "gadu.h"
#include "misc.h"

class PendingMsgs : public QObject
{
	Q_OBJECT

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
		
	public:
		PendingMsgs();
		void deleteMsg(int index);
		bool pendingMsgs(UinType uin);
		bool pendingMsgs();
		int count();
		Element &operator[](int index);
		void addMsg(UinsList uins, QString msg, int msgclass, time_t time);
		bool loadFromFile();
		void writeToFile();

	public slots:
		void openMessages();
		
	signals:
		void messageAdded();
		void messageDeleted();
};

extern PendingMsgs pending;

#endif
