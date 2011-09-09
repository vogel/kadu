#ifndef GROWL_NOTIFY_H
#define GROWL_NOTIFY_H

#include "notify/notifier.h"
#include "gui/windows/main-configuration-window.h"
#include "plugins/generic-plugin.h"

/**
 * @defgroup growl_notify Growl notify
 * @{
 */

class GrowlNotifier;
class GrowlNotifyConfigurationWidget;


class GrowlNotify : public Notifier, GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

public:
	GrowlNotify(QObject *parent = 0);
	~GrowlNotify();

	virtual int init(bool firstLoad);
	virtual void done();

	virtual void notify(Notification *notification);
	virtual CallbackCapacity callbackCapacity() { return CallbackNotSupported; }
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0);
	void createDefaultConfiguration();

	static GrowlNotify *instance() { return Instance; }

private:	
	GrowlNotifier* growlNotifier;
	GrowlNotifyConfigurationWidget *configurationWidget;
	
	QString toPlainText(const QString &text);
	QString parseText(const QString &text, Notification *notification, const QString &def);

	static GrowlNotify *Instance;
};

/** @} */

#endif
