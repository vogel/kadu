#ifndef SINGLE_WINDOW_H
#define SINGLE_WINDOW_H

#include <QtCore/QList>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QMainWindow>

#include "chat_manager.h"
#include "main_configuration_window.h"
#include "configuration_aware_object.h"

class SingleWindow : public QMainWindow, public ChatContainer

{
	Q_OBJECT

	QSplitter *split;
	QTabWidget *tabs;
	QList<int> splitSizes;
	int rosterPos;

protected:
	void closeEvent(QCloseEvent *event);
	void resizeEvent(QResizeEvent *event);

public:
	SingleWindow();
	~SingleWindow();
	void closeChatWidget(ChatWidget *);
	int rosterPosition() { return rosterPos; }

public slots:
	void onOpenChat(ChatWidget *w);
	void onNewChat(ChatWidget *w, bool &handled);
	void onNewMessage(ChatWidget *w);
	void onTabChange(int index);
	void onChatKeyPressed(QKeyEvent *e, ChatWidget* w, bool &handled);
	void onkaduKeyPressed(QKeyEvent *e);
	void closeTab(int index);
};

class SingleWindowManager : public ConfigurationUiHandler, public ConfigurationAwareObject
{
	Q_OBJECT

	SingleWindow *singleWindow;

	protected:
		virtual void configurationUpdated();

	public:
		SingleWindowManager();
		~SingleWindowManager();

		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) {};
};

extern SingleWindowManager *singleWindowManager;

#endif /* SingleWindow_H */
