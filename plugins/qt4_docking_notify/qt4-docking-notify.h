#ifndef QT4_DOCKING_NOTIFY_H
#define QT4_DOCKING_NOTIFY_H

#include "chat/chat.h"
#include "gui/windows/main-configuration-window.h"
#include "notify/notifier.h"

#include "qt4-docking-notify-configuration-widget.h"

/**
 * @defgroup qt4_notify Qt4 Notify
 * @{
 */
class Qt4Notify : public Notifier
{
	Q_OBJECT

	void createDefaultConfiguration();
	QString toPlainText(const QString &text);
	QString parseText(const QString &text, Notification *notification, const QString &def);
	Chat chat;
	Qt4NotifyConfigurationWidget *configurationWidget;

public:
	explicit Qt4Notify(QObject *parent = 0);
	virtual ~Qt4Notify();

	virtual void notify(Notification *notification);

	virtual CallbackCapacity callbackCapacity() { return CallbackNotSupported; }

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0);

public slots:
	void messageClicked();
};

/** @} */

#endif
