#ifndef SINGLE_WINDOW_H
#define SINGLE_WINDOW_H

#include <QtGui/QWidget>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>

#include "chat_manager.h"

class SingleWindow : public QWidget, public ChatContainer
{
	Q_OBJECT
	QSplitter *split;
	QTabWidget *tabs;

protected:
	void closeEvent(QCloseEvent *event);
	void resizeEvent(QResizeEvent *event);

public:
	SingleWindow();
	~SingleWindow();
	void closeChatWidget(ChatWidget *);

public slots:
	void onOpenChat(ChatWidget *);
	void onNewChat(ChatWidget *,bool &);
	void onNewMessage(ChatWidget *);
	void onTabChange(int index);
	void closeTab(int index);
};

extern SingleWindow *singleWindow;

#endif /* SingleWindow_H */
