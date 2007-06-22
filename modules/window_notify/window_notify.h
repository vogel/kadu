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

		virtual void notify(Notification *notification);

		virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }

		void copyConfiguration(const QString &fromEvent, const QString &toEvent) {}

		virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0) { return 0; };
};

extern WindowNotify *window_notify;

/** @} */

#endif
