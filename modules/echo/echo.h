#ifndef ECHO_H
#define ECHO_H

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"
#include "protocols/protocol.h"
#include "contacts/contact.h"

class Echo : public QObject, AccountsAwareObject
{
	Q_OBJECT

	public:
		Echo();
		~Echo();

	protected:
		virtual void accountRegistered(Account account);
		virtual void accountUnregistered(Account account);

	public slots:
		void receivedMessageFilter(Chat chat, Contact sender, const QString &message, time_t time, bool &ignore);
};

extern Echo* echo;

#endif
