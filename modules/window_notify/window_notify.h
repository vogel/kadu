#ifndef WINDOW_NOTIFY_H
#define WINDOW_NOTIFY_H
#include <qobject.h>
#include <qstring.h>

#include "gadu.h"
#include "../notify/notify.h"

/**
 * @defgroup window_notify Window notify
 * @{
 */
class WindowNotify : public Notifier
{
	Q_OBJECT
	public:
		WindowNotify(QObject *parent=0, const char *name=0);
		~WindowNotify();

		virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }

		void copyConfiguration(const QString &fromEvent, const QString &toEvent) {}

	public slots:
		void newChat(Protocol *protocol, UserListElements senders, const QString &msg, time_t t);
		void newMessage(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool &grab);
		void connectionError(Protocol *protocol, const QString &message);
		void userChangedStatusToAvailable(const QString &protocolName, UserListElement);
		void userChangedStatusToBusy(const QString &protocolName, UserListElement);
		void userChangedStatusToInvisible(const QString &protocolName, UserListElement);
		void userChangedStatusToNotAvailable(const QString &protocolName, UserListElement);

		void message(const QString &from, const QString &message, const QMap<QString, QVariant> *parameters, const UserListElement *ule);
		virtual void externalEvent(Notification *notification);
};

extern WindowNotify *window_notify;

/** @} */

#endif
