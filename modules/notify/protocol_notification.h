#ifndef ACCOUNT_NOTIFICATION_H
#define ACCOUNT_NOTIFICATION_H

#include "accounts/account.h"
#include "kadu_parser.h"
#include "notification.h"

class AccountNotification : public Notification
{
	Q_OBJECT

	Account *CurrentAccount;
public:

	AccountNotification(const QString &type, const QPixmap &icon, const ContactList &contacts, Account *account);
	virtual ~AccountNotification();

	Account * account() const {return CurrentAccount;}
};

#endif // ACCOUNT_NOTIFICATION_H
