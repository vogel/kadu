#pragma once

#include <QtCore/QList>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

#include "chat/chat-manager.h"
#include "configuration/configuration-aware-object.h"
#include "configuration/gui/configuration-ui-handler.h"
#include "gui/widgets/custom-input.h"
#include "os/generic/compositing-aware-object.h"
#include "provider/simple-provider.h"

class QSplitter;
class QTabWidget;

class Application;
class ChatConfigurationHolder;
class ChatWidget;
class ChatWidgetSetTitle;
class Configuration;
class FileTransferManager;
class InjectedFactory;

enum class OpenChatActivation;

class SingleWindow : public QWidget, ConfigurationAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE SingleWindow(QWidget *parent = nullptr);
	virtual ~SingleWindow();

	ChatWidget * addChat(Chat chat, OpenChatActivation activation);
	void removeChat(Chat chat);

	virtual void changeEvent(QEvent *event);

	bool isChatWidgetActive(const ChatWidget *chatWidget);
	void tryActivateChatWidget(ChatWidget *chatWidget);

	int rosterPosition() { return m_rosterPos; }
	void changeRosterPos(int newRosterPos);

signals:
	void chatWidgetActivated(ChatWidget *chatWidget);

public slots:
	void onTabChange(int index);
	void onChatKeyPressed(QKeyEvent *e, CustomInput *w, bool &handled);
	void onkaduKeyPressed(QKeyEvent *e);
	void closeTab(ChatWidget *chatWidget);
	void closeTab(int index);
	void onTitleChanged(ChatWidget *chatWidget);
	void closeChat();

protected:
    virtual void configurationUpdated() override;

	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);

private:
	QPointer<Application> m_application;
	QPointer<ChatConfigurationHolder> m_chatConfigurationHolder;
	QPointer<Configuration> m_configuration;
	QPointer<FileTransferManager> m_fileTransferManager;
	QPointer<InjectedFactory> m_injectedFactory;

	QSplitter *m_split;
	QTabWidget *m_tabs;
	QList<int> m_splitSizes;
	int m_rosterPos;
	ChatWidgetSetTitle *m_title;

	void updateTabTitle(ChatWidget *chatWidget);
	void setConfiguration(ChatWidget *chatWidget);

private slots:
	INJEQT_SET void setApplication(Application *application);
	INJEQT_SET void setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setFileTransferManager(FileTransferManager *fileTransferManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

	void titleChanged();

};

class SingleWindowManager : public QObject, public ConfigurationAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SingleWindowManager(QObject *parent = nullptr);
	virtual ~SingleWindowManager();

protected:
	virtual void configurationUpdated();

private:
	QPointer<Configuration> m_configuration;
	QPointer<SingleWindow> m_singleWindow;

	std::shared_ptr<SimpleProvider<QWidget *>> m_windowProvider;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setSingleWindow(SingleWindow *singleWindow);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

};
