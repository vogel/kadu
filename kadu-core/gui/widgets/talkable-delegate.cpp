/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Dariusz Markowicz (darom@alari.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QAbstractItemView>

#include "accounts/account.h"
#include "buddies/buddy-preferred-manager.h"
#include "chat/message/pending-messages-manager.h"
#include "contacts/contact-manager.h"
#include "model/kadu-abstract-model.h"
#include "model/model-chain.h"

#include "talkable-delegate.h"

TalkableDelegate::TalkableDelegate(TalkableTreeView *parent) :
		KaduTreeViewDelegate(parent), Chain(0)
{
	connect(ContactManager::instance(), SIGNAL(contactUpdated(Contact&)), this, SLOT(contactUpdated(Contact&)));
	connect(BuddyPreferredManager::instance(), SIGNAL(buddyUpdated(Buddy&)), this, SLOT(buddyUpdated(Buddy&)));
	connect(PendingMessagesManager::instance(), SIGNAL(messageAdded(Message)), this, SLOT(messageStatusChanged(Message)));
	connect(PendingMessagesManager::instance(), SIGNAL(messageRemoved(Message)), this, SLOT(messageStatusChanged(Message)));
}

TalkableDelegate::~TalkableDelegate()
{
	disconnect(ContactManager::instance(), SIGNAL(contactUpdated(Contact&)), this, SLOT(contactUpdated(Contact&)));
	disconnect(BuddyPreferredManager::instance(), SIGNAL(buddyUpdated(Buddy&)), this, SLOT(buddyUpdated(Buddy&)));
	disconnect(PendingMessagesManager::instance(), SIGNAL(messageAdded(Message)), this, SLOT(messageStatusChanged(Message)));
	disconnect(PendingMessagesManager::instance(), SIGNAL(messageRemoved(Message)), this, SLOT(messageStatusChanged(Message)));
}

void TalkableDelegate::setChain(ModelChain *chain)
{
	if (Chain)
		disconnect(Chain, SIGNAL(destroyed(QObject *)), this, SLOT(chainDestroyed()));

	Chain = chain;

	if (Chain)
		connect(Chain, SIGNAL(destroyed(QObject *)), this, SLOT(chainDestroyed()));
}

void TalkableDelegate::contactUpdated(Contact &contact)
{
	if (!Chain)
		return;

	const QModelIndexList &contactsIndexList = Chain->indexListForValue(contact);
	foreach (const QModelIndex &contactIndex, contactsIndexList)
		emit sizeHintChanged(contactIndex);

	const Buddy &buddy = contact.ownerBuddy();
	const QModelIndexList &buddyIndexList = Chain->indexListForValue(buddy);
	const int contactIndexInBuddy = buddy.contacts().indexOf(contact);

	foreach (const QModelIndex &buddyIndex, buddyIndexList)
	{
		const QModelIndex &contactIndex = buddyIndex.child(contactIndexInBuddy, 0);

		emit sizeHintChanged(buddyIndex);
		emit sizeHintChanged(contactIndex);
	}
}

void TalkableDelegate::buddyUpdated(Buddy &buddy)
{
	if (!Chain)
		return;

	const QModelIndexList &buddyIndexList = Chain->indexListForValue(buddy);

	foreach (const QModelIndex &buddyIndex, buddyIndexList)
		emit sizeHintChanged(buddyIndex);
}

void TalkableDelegate::messageStatusChanged(Message message)
{
	Buddy buddy = message.messageSender().ownerBuddy();
	buddyUpdated(buddy);
}

void TalkableDelegate::chainDestroyed()
{
	Chain = 0;
}
