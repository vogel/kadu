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
		PendingMsgs(QObject *parent=0, const char *name=0);
		bool pendingMsgs(UinType uin);
		bool pendingMsgs();
		int count();
		Element &operator[](int index);

	public slots:
		void openMessages();
		void deleteMsg(int index);
		void addMsg(UinsList uins, QString msg, int msgclass, time_t time);
		bool loadFromFile();
		void writeToFile();
		
	signals:
		void messageAdded();
		void messageDeleted();
};

extern PendingMsgs pending;

#endif
