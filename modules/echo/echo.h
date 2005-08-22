#ifndef ECHO_H
#define ECHO_H

#include <qobject.h>

#include "gadu.h"
#include "protocol.h"
#include "usergroup.h"

class Echo : public QObject
{
	Q_OBJECT

	public:
		Echo();
		~Echo();

	public slots:
		void chatMsgReceived(Protocol *protocol, UserListElements senders, const QString& msg, time_t time, bool& grab);
};

extern Echo* echo;

#endif
