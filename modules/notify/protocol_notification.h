#ifndef PROTOCOL_NOTIFICATION_H
#define PROTOCOL_NOTIFICATION_H

#include "kadu_parser.h"

#include "notification.h"

class ProtocolNotification : public Notification
{
	Q_OBJECT

	QString ProtocolName;
public:

	ProtocolNotification(const QString &type, const QString &icon, const UserListElements &userListElements, const QString &protocolName);
	virtual ~ProtocolNotification();

	/**
		Ustawia nazwę protokołu.
	 **/
	void setProtocolName(const QString &name);
	/**
		Nazwa protokołu.

		@return nazwa protokołu.
	 **/
	QString protocolName() const;
};

#endif // PROTOCOL_NOTIFICATION_H
