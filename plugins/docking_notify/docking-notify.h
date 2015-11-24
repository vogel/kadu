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
class DockingNotify : public Notifier
{
	Q_OBJECT

	void createDefaultConfiguration();
	QString toPlainText(const QString &text);
	QString parseText(const QString &text, Notification *notification, const QString &def);
	Chat chat;
	DockingNotifyConfigurationWidget *configurationWidget;

public:
	explicit DockingNotify(QObject *parent = 0);
	virtual ~DockingNotify();

	virtual void notify(Notification *notification);

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0);

public slots:
	void messageClicked();
};

/** @} */

#endif
