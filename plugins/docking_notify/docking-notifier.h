#pragma once

#include "chat/chat.h"
#include "gui/windows/main-configuration-window.h"
#include "notification/notifier.h"

#include "docking-notify-configuration-widget.h"

#include <injeqt/injeqt.h>

class ChatWidgetManager;
class Configuration;
class Docking;
class InjectedFactory;
class Parser;

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

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = nullptr);

public slots:
	void messageClicked();

private:
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<Configuration> m_configuration;
	QPointer<Docking> m_docking;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<Parser> m_parser;

	void createDefaultConfiguration();
	QString toPlainText(const QString &text);
	QString parseText(const QString &text, Notification *notification, const QString &def);
	Chat chat;
	DockingNotifyConfigurationWidget *configurationWidget;

private slots:
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setDocking(Docking *docking);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setParser(Parser *parser);
	INJEQT_INIT void init();

};

/** @} */
