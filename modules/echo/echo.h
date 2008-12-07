#ifndef ECHO_H
#define ECHO_H

#include <QtCore/QObject>

#include "../modules/gadu_protocol/gadu.h"
#include "protocols/protocol.h"
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
