/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "chat/type/chat-type-contact-set.h"
#include "chat/type/chat-type-contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "formatted-string/formatted-string-factory.h"
#include "message/message.h"
#include "plugins/history/history.h"
#include "status/status.h"

#include "history-import-thread.h"
#include "history-importer-manager.h"
#include "history-migration-helper.h"

HistoryImportThread::HistoryImportThread(Account gaduAccount, const QString &path, const QList<UinsList> &uinsLists, QObject *parent) :
		QObject(parent), GaduAccount(gaduAccount), Path(path), UinsLists(uinsLists),
		ImportedEntries(0), ImportedChats(0), TotalMessages(0),
		ImportedMessages(0), Canceled(false), CancelForced(false)
{
}

HistoryImportThread::~HistoryImportThread()
{
}

void HistoryImportThread::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	CurrentFormattedStringFactory = formattedStringFactory;
}

void HistoryImportThread::prepareChats()
{
	foreach (const UinsList &uinsList, UinsLists)
		chatFromUinsList(uinsList);
}

void HistoryImportThread::run()
{
	History::instance()->setSyncEnabled(false);

	ImportedEntries = 0;

	foreach (const UinsList &uinsList, UinsLists)
	{
		if (Canceled)
			break;

		ImportedChats++;

		Chat chat = chatFromUinsList(uinsList);
		// we cannot import into non-existing chat
		// this means chat with ourself on the list
		if (!chat || !chat.data())
			continue;

		QList<HistoryEntry> entries = HistoryMigrationHelper::historyEntries(Path, uinsList);

		if (chat.property("history-importer:Imported", false).toBool())
		{
			ImportedEntries += entries.count();
			continue;
		}

		ImportedMessages = 0;
		TotalMessages = entries.count();

		if (Canceled)
			break;

		foreach (const HistoryEntry &entry, entries)
		{
			if (Canceled && CancelForced)
				break;
			importEntry(chat, entry);
			ImportedMessages++;
		}

		if (Canceled && CancelForced)
			break;

		chat.addProperty("history-importer:Imported", true, CustomProperties::Storable);
		// force sync for every chat
		History::instance()->forceSync();
	}

	History::instance()->setSyncEnabled(true);

	emit finished();
}

void HistoryImportThread::cancel(bool force)
{
	Canceled = true;
	CancelForced = force;
}

Chat HistoryImportThread::chatFromUinsList(const UinsList &uinsList) const
{
	ContactSet contacts;
	foreach (UinType uin, uinsList)
		contacts.insert(ContactManager::instance()->byId(GaduAccount, QString::number(uin), ActionCreateAndAdd));

	if (contacts.isEmpty())
		return Chat::null;

	// it is called before this object is move to separate thread, so it is safe to add it to ChatManager later
	return 1 == contacts.size()
			? ChatTypeContact::findChat(*contacts.constBegin(), ActionCreateAndAdd)
			: ChatTypeContactSet::findChat(contacts, ActionCreateAndAdd);
}

void HistoryImportThread::importEntry(const Chat &chat, const HistoryEntry &entry)
{
	if (!CurrentFormattedStringFactory)
		return;

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
			msg.setContent(CurrentFormattedStringFactory->fromHtml(entry.Content));
			msg.setSendDate(entry.SendDate);
			msg.setReceiveDate(entry.Date);
			msg.setType(outgoing ? MessageTypeSent : MessageTypeReceived);

			History::instance()->currentStorage()->appendMessage(msg);
			ImportedEntries++;
			break;
		}
		case HistoryEntry::StatusChange:
		{
			StatusType statusType;
			switch (entry.Status)
			{
				case HistoryEntry::Online:
					statusType = StatusTypeOnline;
					break;
				case HistoryEntry::Busy:
					statusType = StatusTypeAway;
					break;
				case HistoryEntry::Invisible:
					statusType = StatusTypeInvisible;
					break;
				case HistoryEntry::FFC:
					statusType = StatusTypeFreeForChat;
					break;
				case HistoryEntry::DND:
					statusType = StatusTypeDoNotDisturb;
					break;
				case HistoryEntry::Offline:
					statusType = StatusTypeOffline;
					break;
				default:
					return;
			}

			Status status(statusType, entry.Content);
			Contact contact = ContactManager::instance()->byId(GaduAccount, QString::number(entry.Uin), ActionCreateAndAdd);
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

#include "moc_history-import-thread.cpp"
