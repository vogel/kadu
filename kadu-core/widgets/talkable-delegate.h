/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "buddies/buddy.h"
#include "message/message.h"
#include "widgets/kadu-tree-view-delegate.h"
#include "widgets/talkable-delegate-configuration.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyPreferredManager;
class ContactManager;
class Contact;
class ModelChain;
class UnreadMessageRepository;

class TalkableDelegate : public KaduTreeViewDelegate
{
	Q_OBJECT

public:
	explicit TalkableDelegate(TalkableTreeView *parent = nullptr);
	virtual ~TalkableDelegate();

	virtual void setChain(ModelChain *chain);

	virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
	QPointer<BuddyPreferredManager> m_buddyPreferredManager;
	QPointer<ContactManager> m_contactManager;
	QPointer<UnreadMessageRepository> m_unreadMessageRepository;

	QPointer<ModelChain> Chain;

private slots:
	INJEQT_SET void setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager);
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_SET void setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository);
	INJEQT_INIT void init();

	void contactUpdated(const Contact &contact);
	void buddyUpdated(const Buddy &buddy);
	void messageStatusChanged(Message message);

};
