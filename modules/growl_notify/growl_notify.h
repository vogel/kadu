#ifndef WINDOW_NOTIFY_H
#define WINDOW_NOTIFY_H

#include "../notify/notify.h"

/**
 * @defgroup growl_notify Growl notify
 * @{
 */

class UserListElements;
class GrowlNotifier;
class GrowlNotifyConfigurationWidget;
class GrowlNotify : public Notifier
{
	Q_OBJECT

	void createDefaultConfiguration();
	
	GrowlNotifier* growlNotifier;
	GrowlNotifyConfigurationWidget *configurationWidget;
	UserListElements senders;
	
	QString toPlainText(const QString &text);
	QString parseText(const QString &text, Notification *notification, const QString &def);

public:
	GrowlNotify(QObject *parent = 0, const char *name = 0);
	~GrowlNotify();

	virtual void notify(Notification *notification);

	virtual CallbackCapacity callbackCapacity() { return CallbackNotSupported; }

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0);

public slots:
	void notification_clicked();
};

extern GrowlNotify *growl_notify;

/** @} */

#endif
