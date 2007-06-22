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
		void messageReceived(Protocol *protocol, UserListElements senders, const QString& msg, time_t time);
};

extern Echo* echo;

#endif
