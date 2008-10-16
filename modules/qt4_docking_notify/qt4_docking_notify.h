#ifndef QT4_DOCKING_NOTIFY_H
#define QT4_DOCKING_NOTIFY_H

#include "gadu.h"
#include "../notify/notify.h"
#include "../qt4_docking/qt4_docking.h"

class UserListElements;

/**
 * @defgroup window_notify Window notify
 * @{
 */
class Qt4Notify : public Notifier
{
	Q_OBJECT

	void createDefaultConfiguration();
	QString toPlainText(const QString &text);
	UserListElements senders;

public:
	Qt4Notify(QObject *parent = 0, const char *name = 0);
	~Qt4Notify();

	virtual void notify(Notification *notification);

	virtual CallbackCapacity callbackCapacity() { return CallbackNotSupported; }

	void copyConfiguration(const QString &fromEvent, const QString &toEvent) {}

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0) { return 0; };

public slots:
	void messageClicked();
};

extern Qt4Notify *qt4_notify;
extern Qt4TrayIcon* qt4_tray_icon;

/** @} */

#endif
