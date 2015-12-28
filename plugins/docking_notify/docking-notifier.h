#ifndef QT4_DOCKING_NOTIFY_H
#define QT4_DOCKING_NOTIFY_H

#include "chat/chat.h"
#include "gui/windows/main-configuration-window.h"
#include "notification/notifier.h"

#include "docking-notify-configuration-widget.h"

/**
 * @defgroup qt4_notify Qt4 Notify
 * @{
 */
class DockingNotifier : public QObject, public Notifier
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit DockingNotifier(QObject *parent = nullptr);
	virtual ~DockingNotifier();

	virtual void notify(Notification *notification);

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0);

private:
	void createDefaultConfiguration();
	QString toPlainText(const QString &text);
	QString parseText(const QString &text, Notification *notification, const QString &def);
	Chat chat;
	DockingNotifyConfigurationWidget *configurationWidget;

public slots:
	void messageClicked();

};

/** @} */

#endif
