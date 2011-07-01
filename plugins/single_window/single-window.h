#ifndef SINGLE_WINDOW_H
#define SINGLE_WINDOW_H

#include <QtCore/QList>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QMainWindow>

#include "chat/chat-manager.h"
#include "configuration/configuration-aware-object.h"
#include "gui/widgets/chat-widget-container.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/main-configuration-window.h"
#include "os/generic/compositing-aware-object.h"

class KaduIcon;

class SingleWindow : public QMainWindow, public ChatWidgetContainer
{
	Q_OBJECT

	QSplitter *split;
	QTabWidget *tabs;
	QList<int> splitSizes;
	int rosterPos;

protected:
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
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
	void onStatusPixmapChanged(const KaduIcon &icon);

};

class SingleWindowManager : public ConfigurationUiHandler, public ConfigurationAwareObject
{
	Q_OBJECT

	SingleWindow *singleWindow;

protected:
	virtual void configurationUpdated();

public:
	explicit SingleWindowManager(QObject *parent = 0);
	virtual ~SingleWindowManager();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow */*mainConfigurationWindow*/) {};

};

#endif /* SingleWindow_H */
