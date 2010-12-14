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

#include "chat/chat.h"
#include "chat/chat-manager.h"
#include "chat/message/message.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "modules/history/history.h"
#include "status/status.h"

#include "history-import-thread.h"
#include "history-migration-helper.h"

HistoryImportThread::HistoryImportThread(Account gaduAccount, const QList<UinsList> &uinsLists, int totalEntries, QObject *parent) :
		QThread(parent), GaduAccount(gaduAccount), UinsLists(uinsLists), Canceled(false), TotalEntries(totalEntries), ImportedEntries(0)
{
}

HistoryImportThread::~HistoryImportThread()
{
}

void HistoryImportThread::run()
{
	ImportedEntries = 0;

	foreach (const UinsList &uinsList, UinsLists)
	{
		if (Canceled)
			break;

		Chat chat = chatFromUinsList(uinsList);
		QList<HistoryEntry> entries = HistoryMigrationHelper::historyEntries(uinsList);

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

Chat HistoryImportThread::chatFromUinsList(const UinsList &uinsList) const
{
	ContactSet contacts;
	foreach (UinType uin, uinsList)
		contacts.insert(ContactManager::instance()->byId(GaduAccount, QString::number(uin), ActionCreateAndAdd));

	return ChatManager::instance()->findChat(contacts);
}

void HistoryImportThread::importEntry(Chat chat, const HistoryEntry &entry)
{
	switch (entry.Type)
	{
		case HistoryEntry::ChatSend:
		case HistoryEntry::MsgSend:
		case HistoryEntry::ChatRcv:
		case HistoryEntry::MsgRcv:
		{
			bool outgoing = (entry.Type == HistoryEntry::ChatSend || entry.Type == HistoryEntry::MsgSend);
			bool isChat = (entry.Type == HistoryEntry::ChatSend || entry.Type == HistoryEntry::ChatRcv);

			if (isChat && !chat)
				return;

			Message msg = Message::create();
			msg.setMessageChat(isChat ? chat : Chat::null);
			msg.setMessageSender(outgoing
					? GaduAccount.accountContact()
					: ContactManager::instance()->byId(GaduAccount, QString::number(entry.Uin), ActionCreateAndAdd));
			msg.setContent(entry.Content);
			msg.setSendDate(entry.SendDate);
			msg.setReceiveDate(entry.Date);
			msg.setType(outgoing ? Message::TypeSent : Message::TypeReceived);

			// TODO 0.6.6: it's damn slow!
			History::instance()->currentStorage()->appendMessage(msg);
			ImportedEntries++;
			break;
		}
		case HistoryEntry::StatusChange:
		{
			QString statusStr;
			switch (entry.Status)
			{
				case HistoryEntry::Online:
					statusStr = "Online";
					break;
				case HistoryEntry::Offline:
					statusStr = "Offline";
					break;
				case HistoryEntry::Busy:
					statusStr = "Away";
					break;
				case HistoryEntry::Invisible:
					statusStr = "Invisible";
					break;
				default:
					return;
			}

			Status status(statusStr, entry.Content);
			Contact contact = ContactManager::instance()->byId(GaduAccount, QString::number(entry.Uin), ActionCreateAndAdd);
			// TODO 0.6.6: it's damn slow!
			History::instance()->currentStorage()->appendStatus(contact, status, entry.Date);
			ImportedEntries++;
			break;
		}
		case HistoryEntry::SmsSend:
			History::instance()->currentStorage()->appendSms(entry.Mobile, entry.Content, entry.Date);
			ImportedEntries++;
			break;
		default:
			break;
	}
}
