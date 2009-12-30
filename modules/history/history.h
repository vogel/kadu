/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef HISTORY_H
#define HISTORY_H

#include <QtCore/QObject>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QKeyEvent>
#include <QtGui/QCheckBox>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtGui/QDialog>

enum HistoryEntryType
{
	EntryTypeMessage = 0x00000001,
	EntryTypeStatus = 0x00000010,
	EntryTypeSms = 0x00000020,
	EntryTypeAll = 0x0000003f
};

#include "configuration/configuration-aware-object.h"
#include "buddies/buddy-remove-predicate-object.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/windows/history-window.h"
#include "gui/windows/main-configuration-window.h"
#include "protocols/protocol.h"
#include "storage/history-storage.h"

#include "history_exports.h"

class Account;
class ChatWidget;
class HistoryWindow;

class HISTORYAPI History : public ConfigurationUiHandler, ConfigurationAwareObject, BuddyRemovePredicateObject
{
	Q_OBJECT

	static History *Instance;
	HistoryStorage *CurrentStorage;
	HistoryWindow *HistoryDialog;

	ActionDescription *ShowHistoryActionDescription;
	ActionDescription *ChatsHistoryActionDescription;
	ActionDescription *ShowMoreMessagesInChatWidgetActionDescription;
	ActionDescription *ClearHistoryActionDescription;

	QLabel *dontCiteOldMessagesLabel;
	QListWidget *allStatusUsers;
	QListWidget *selectedStatusUsers;
	QListWidget *allChatsUsers;
	QListWidget *selectedChatsUsers; 

	History();
	virtual ~History();

	void createActionDescriptions();
	void deleteActionDescriptions();
	virtual void configurationUpdated();
	virtual bool removeContactFromStorage(Buddy buddy);
	void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

private slots:
	void accountRegistered(Account);
	void accountUnregistered(Account);

	void showHistoryActionActivated(QAction *sender, bool toggled);
	void showMoreMessages(QAction *action);
	void clearHistoryActionActivated(QAction *sender, bool toggled);

	void chatCreated(ChatWidget *chatWidget);

	void updateQuoteTimeLabel(int value);
	void configurationWindowApplied();

public:
	static History * instance();
	HistoryStorage * currentStorage() { return CurrentStorage; }
	void registerStorage(HistoryStorage *storage);
	void unregisterStorage(HistoryStorage *storage);
	QList<Chat> chatsList(HistorySearchParameters search);
	QList<QDate> datesForChat(Chat chat, HistorySearchParameters search);
	QList<Message> messages(Chat chat, QDate date = QDate(), int limit = 0);
	int messagesCount(Chat chat, QDate date = QDate());

};

void disableNonHistoryContacts(Action *action);

#endif
