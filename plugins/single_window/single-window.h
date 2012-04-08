#ifndef SINGLE_WINDOW_H
#define SINGLE_WINDOW_H

#include <QtCore/QList>
#include <QtGui/QMainWindow>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>

#include "chat/chat-manager.h"
#include "configuration/configuration-aware-object.h"
#include "gui/widgets/chat-widget-container.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/main-configuration-window.h"
#include "os/generic/compositing-aware-object.h"
#include "provider/simple-provider.h"

class SingleWindow : public QMainWindow, public ChatWidgetContainer
{
	Q_OBJECT

	QSplitter *split;
	QTabWidget *tabs;
	QList<int> splitSizes;
	int rosterPos;

	void updateTabIcon(ChatWidget *chatWidget);
	void updateTabName(ChatWidget *chatWidget);

protected:
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);

public:
	SingleWindow();
	~SingleWindow();

	virtual void changeEvent(QEvent *event);

	virtual void activateChatWidget(ChatWidget *chatWidget);
	virtual void alertChatWidget(ChatWidget *chatWidget);
	virtual void closeChatWidget(ChatWidget *chatWidget);
	virtual bool isChatWidgetActive(ChatWidget *chatWidget);

	int rosterPosition() { return rosterPos; }
	void changeRosterPos(int newRosterPos);

public slots:
	void onNewChat(ChatWidget *chatWidget, bool &handled);
	void onTabChange(int index);
	void onChatKeyPressed(QKeyEvent *e, CustomInput *w, bool &handled);
	void onkaduKeyPressed(QKeyEvent *e);
	void closeTab(int index);
	void onIconChanged();
	void onTitleChanged(ChatWidget *chatWidget, const QString &newTitle);
	void closeChat();

};

class SingleWindowManager : public ConfigurationUiHandler, public ConfigurationAwareObject
{
	Q_OBJECT

	QSharedPointer<SimpleProvider<QWidget *> > WindowProvider;
	SingleWindow *Window;

protected:
	virtual void configurationUpdated();

public:
	explicit SingleWindowManager(QObject *parent = 0);
	virtual ~SingleWindowManager();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow * /*mainConfigurationWindow*/) {};

};

#endif /* SingleWindow_H */
