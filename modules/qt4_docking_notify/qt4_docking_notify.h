#ifndef QT4_DOCKING_NOTIFY_H
#define QT4_DOCKING_NOTIFY_H

#include "gadu.h"
#include "configuration_aware_object.h"
#include "main_configuration_window.h"
#include "../notify/notify.h"
#include "../qt4_docking/qt4_docking.h"

#include "qt4_docking_notify_configuration_widget.h"

class UserListElements;

/**
 * @defgroup qt4_notify Qt4 Notify
 * @{
 */
class Qt4Notify : public Notifier, public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	void createDefaultConfiguration();
	QString toPlainText(const QString &text);
	QString parseText(const QString &text, Notification *notification, const QString &def);
	UserListElements senders;
	Qt4NotifyConfigurationWidget *configurationWidget;

public:
	Qt4Notify(QObject *parent = 0, const char *name = 0);
	~Qt4Notify();

	virtual void notify(Notification *notification);

	virtual CallbackCapacity callbackCapacity() { return CallbackNotSupported; }

	void copyConfiguration(const QString &fromEvent, const QString &toEvent) {}

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0);
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow*);
	virtual void configurationUpdated();

public slots:
	void messageClicked();
};

extern Qt4Notify *qt4_notify;
extern Qt4TrayIcon* qt4_tray_icon;

/** @} */

#endif
