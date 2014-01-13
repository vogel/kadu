#pragma once

#include <QtCore/QList>
#include <QtGui/QWidget>

#include "chat/chat-manager.h"
#include "configuration/configuration-aware-object.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/main-configuration-window.h"
#include "os/generic/compositing-aware-object.h"
#include "provider/simple-provider.h"

class QSplitter;
class QTabWidget;

class ChatWidget;

class SingleWindow : public QWidget
{
	Q_OBJECT

public:
	SingleWindow();
	~SingleWindow();

	void addChatWidget(ChatWidget *chatWidget);
	void removeChatWidget(ChatWidget *chatWidget);

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
	void closeTab(int index);
	void onIconChanged();
	void onTitleChanged(ChatWidget *chatWidget, const QString &newTitle);
	void closeChat();

protected:
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);

private:
	QSplitter *m_split;
	QTabWidget *m_tabs;
	QList<int> m_splitSizes;
	int m_rosterPos;

	void updateTabIcon(ChatWidget *chatWidget);
	void updateTabName(ChatWidget *chatWidget);

private slots:
	void unreadMessagesCountChanged(ChatWidget *chatWidget);

};

class SingleWindowManager : public ConfigurationUiHandler, public ConfigurationAwareObject
{
	Q_OBJECT

public:
	explicit SingleWindowManager(QObject *parent = 0);
	virtual ~SingleWindowManager();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow * /*mainConfigurationWindow*/) {};

	SingleWindow * window() const { return m_window; }

protected:
	virtual void configurationUpdated();

private:
	std::shared_ptr<SimpleProvider<QWidget *>> m_windowProvider;
	SingleWindow *m_window;

};
