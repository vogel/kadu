#ifndef HISTORY_MODULE_H
#define HISTORY_MODULE_H

#include "main_configuration_window.h"
#include "usergroup.h"
//Added by qt3to4:
#include <QKeyEvent>
#include <QLabel>

class QLabel;

class ActionDescription;
class ChatWidget;
class UserGroup;

class HistoryModule : public ConfigurationUiHandler
{
	Q_OBJECT

	ActionDescription *historyActionDescription;

	QLabel *dontCiteOldMessagesLabel;

	void createDefaultConfiguration();
	void appendHistory(ChatWidget *chat);

private slots:
	void historyActionActivated(const UserGroup* users);
	void messageSentAndConfirmed(UserListElements receivers, const QString& message);
	void viewHistory();
	void deleteHistory();
	void userboxMenuPopup();
	void removingUsers(UserListElements users);
	void updateQuoteTimeLabel(int);

	void chatKeyPressed(QKeyEvent *e, ChatWidget *widget, bool &handled);

	void chatCreated(ChatWidget *chat);
	void chatDestroying(ChatWidget *chat);

public:
	HistoryModule();
	virtual ~HistoryModule();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
};

extern HistoryModule* history_module;

#endif
