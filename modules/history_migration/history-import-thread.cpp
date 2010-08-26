/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QDateTime>
#include <QtCore/QStringList>

#include "accounts/account.h"
#include "chat/chat.h"
#include "chat/chat-manager.h"
#include "chat/message/message.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "modules/history/history.h"

#include "history-import-thread.h"
#include "history-migration-helper.h"

HistoryImportThread::HistoryImportThread(Account gaduAccount, QList<QStringList> uinsLists, unsigned long totalEntries, QObject *parent) :
		QThread(parent), GaduAccount(gaduAccount), UinsLists(uinsLists), Canceled(false), TotalEntries(totalEntries), ImportedEntries(0)
{
}

HistoryImportThread::~HistoryImportThread()
{
}

void HistoryImportThread::run()
{
	ImportedEntries = 0;

	foreach (QStringList uinsList, UinsLists)
	{
		if (Canceled)
			break;

		Chat chat = chatFromUinsList(uinsList);
		if (!chat)
			continue;

		QList<HistoryEntry> entries = HistoryMigrationHelper::historyEntries(uinsList, HISTORYMANAGER_ENTRY_CHATSEND | HISTORYMANAGER_ENTRY_CHATRCV);

		foreach (const HistoryEntry &entry, entries)
			if (Canceled)
				break;
			else
				importEntry(chat, entry);
	}
}

void HistoryImportThread::cancel()
{
	Canceled = true;
}

Chat HistoryImportThread::chatFromUinsList(QStringList uinsList) const
{
	ContactSet contacts;
	foreach (const QString &uin, uinsList)
		contacts.insert(ContactManager::instance()->byId(GaduAccount, uin, ActionCreateAndAdd));

	return ChatManager::instance()->findChat(contacts);
}

void HistoryImportThread::importEntry(Chat chat, const HistoryEntry &entry)
{
	QString id = QString::number(entry.uin);

	bool outgoing = entry.type == HISTORYMANAGER_ENTRY_CHATSEND;
	QDateTime sendTime = entry.sdate;
	QDateTime recieveTime = entry.date;
	QString messageString = entry.message;

	Message msg = Message::create();
	msg.setMessageChat(chat);
	msg.setMessageSender(outgoing
			? GaduAccount.accountContact()
			: ContactManager::instance()->byId(GaduAccount, id, ActionCreateAndAdd));
	msg.setContent(entry.message);
	msg.setSendDate(entry.sdate);
	msg.setReceiveDate(entry.date);

	//TODO 0.6.6: it's damn slow!
	History::instance()->currentStorage()->appendMessage(msg);
	ImportedEntries++;
}
