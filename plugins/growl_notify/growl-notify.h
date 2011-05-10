#ifndef WINDOW_NOTIFY_H
#define WINDOW_NOTIFY_H

#include "notify/notifier.h"

/**
 * @defgroup growl_notify Growl notify
 * @{
 */

class GrowlNotifier;
class GrowlNotifyConfigurationWidget;

class GrowlNotify : public Notifier
{
	Q_OBJECT

	void createDefaultConfiguration();
	
	GrowlNotifier* growlNotifier;
	GrowlNotifyConfigurationWidget *configurationWidget;
	
	QString toPlainText(const QString &text);
	QString parseText(const QString &text, Notification *notification, const QString &def);

public:
	GrowlNotify(QObject *parent = 0);
	~GrowlNotify();

	virtual void notify(Notification *notification);

	virtual CallbackCapacity callbackCapacity() { return CallbackNotSupported; }

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0);
};

/** @} */

#endif
