#ifndef SINGLE_WINDOW_H
#define SINGLE_WINDOW_H

#include <QtCore/QList>
#include <QtGui/QIcon>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QMainWindow>

#include "chat/chat-manager.h"
#include "gui/windows/main-configuration-window.h"
#include "configuration/configuration-aware-object.h"
#include "gui/widgets/chat-widget.h"
#include "os/generic/compositing-aware-object.h"

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
	void changeRosterPos(int newRosterPos);

public slots:
	void onOpenChat(ChatWidget *w);
	void onNewChat(ChatWidget *w, bool &handled);
	void onNewMessage(Chat chat);
	void onTabChange(int index);
	void onChatKeyPressed(QKeyEvent *e, CustomInput *w, bool &handled);
	void onkaduKeyPressed(QKeyEvent *e);
	void closeTab(int index);
	void onIconChanged();
	void onStatusPixmapChanged(const QIcon &icon);
	void showHide();
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

		virtual void mainConfigurationWindowCreated(MainConfigurationWindow */*mainConfigurationWindow*/) {};
};

extern SingleWindowManager *singleWindowManager;

#endif /* SingleWindow_H */
