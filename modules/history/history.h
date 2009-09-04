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
#include "contacts/contact-remove-predicate-object.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/contacts-list-widget-menu-manager.h"
#include "gui/windows/history-dialog.h"
#include "gui/windows/main-configuration-window.h"
#include "protocols/protocol.h"
#include "storage/history-storage.h"

#include "history_exports.h"

class Account;
class HistoryDlg;

class HISTORYAPI History : public ConfigurationUiHandler, ConfigurationAwareObject, ContactRemovePredicateObject
{
	Q_OBJECT

	static History *Instance;
	HistoryStorage *CurrentStorage;
	HistoryDlg *HistoryDialog;

	ActionDescription *ShowHistoryActionDescription;
	ActionDescription *ChatsHistoryActionDescription;
	ActionDescription *ShowMoreMessagesInChatWidgetActionDescription;

	QLabel *dontCiteOldMessagesLabel;
	QListWidget *allStatusUsers;
	QListWidget *selectedStatusUsers;
	QListWidget *allChatsUsers;
	QListWidget *selectedChatsUsers; 

	History();
	~History();

	void createActionDescriptions();
	void deleteActionDescriptions();
	virtual void configurationUpdated();
	virtual bool removeContactFromStorage(Contact contact);
	void configurationWindowApplied();
	void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	void updateQuoteTimeLabel(int value);

private slots:
	void accountRegistered(Account *);
	void accountUnregistered(Account *);

	void showHistoryActionActivated(QAction *sender, bool toggled);
	void showMoreMessagesActionActivated(QAction *sender, bool toggled);

public:
	static History * instance();
	HistoryStorage * currentStorage() { return CurrentStorage; }
	void registerStorage(HistoryStorage *storage);
	void unregisterStorage(HistoryStorage *storage);
	QList<Chat *> chatsList();
	QList<QDate> datesForChat(Chat *chat);
	QList<Message> getMessages(Chat *chat, QDate date = QDate(), int limit = 0);
	int getMessagesCount(Chat *chat, QDate date = QDate());

};

	void disableNonHistoryContacts(Action *action);

#endif
