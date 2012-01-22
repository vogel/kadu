/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2007 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HISTORY_H
#define HISTORY_H

#include <QtCore/QDateTime>
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QQueue>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>

#include "configuration/configuration-aware-object.h"
#include "core/crash-aware-object.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/windows/main-configuration-window.h"
#include "message/message.h"
#include "protocols/protocol.h"
#include "storage/history-storage.h"

#include "history_exports.h"

class Account;
class ChatWidget;
class HistorySaveThread;
class HistoryWindow;
class ShowHistoryActionDescription;

class HISTORYAPI History : public ConfigurationUiHandler, ConfigurationAwareObject, CrashAwareObject
{
	Q_OBJECT

	static History *Instance;

	bool SaveChats;
	bool SaveChatsWithAnonymous;
	bool SaveStatuses;
	bool SaveOnlyStatusesWithDescription;
	bool SyncEnabled;

	int ChatHistoryCitation;
	int ChatHistoryQuotationTime;

	QMutex UnsavedDataMutex;
	QQueue<Message> UnsavedMessages;
	QQueue<QPair<Contact, Status> > UnsavedStatusChanges;
	HistorySaveThread *SaveThread;

	HistoryStorage *CurrentStorage;

	ShowHistoryActionDescription *ShowHistoryActionDescriptionInstance;
	ActionDescription *ClearHistoryActionDescription;

	QLabel *dontCiteOldMessagesLabel;
	QListWidget *allStatusUsers;
	QListWidget *selectedStatusUsers;
	QListWidget *allChatsUsers;
	QListWidget *selectedChatsUsers;

	History();
	virtual ~History();

	void startSaveThread();
	void stopSaveThread();

	void createDefaultConfiguration();

	void createActionDescriptions();
	void deleteActionDescriptions();
	virtual void configurationUpdated();
	void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	friend class HistorySaveThread;
	Message dequeueUnsavedMessage();
	QPair<Contact, Status> dequeueUnsavedStatusChange();

private slots:
	void accountRegistered(Account);
	void accountUnregistered(Account);

	void enqueueMessage(const Message &);
	void contactStatusChanged(Contact contact, Status oldStatus);

	void clearHistoryActionActivated(QAction *sender, bool toggled);

	void chatCreated(ChatWidget *chatWidget);

	void updateQuoteTimeLabel(int value);

protected:
	virtual void crash();

public:
	static void createInstance();
	static void destroyInstance();
	static History * instance();

	HistoryStorage * currentStorage() { return CurrentStorage; }
	void registerStorage(HistoryStorage *storage);
	void unregisterStorage(HistoryStorage *storage);

	void forceSync();
	void setSyncEnabled(bool syncEnabled);

signals:
	void storageChanged(HistoryStorage *newStorage);

};

void disableNonHistoryContacts(Action *action);

#endif // HISTORY_H
