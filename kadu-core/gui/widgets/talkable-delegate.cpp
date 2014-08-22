/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Dariusz Markowicz (darom@alari.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QAbstractItemView>
#include <QtGui/QKeyEvent>

#include "accounts/account.h"
#include "buddies/buddy-preferred-manager.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "gui/widgets/talkable-painter.h"
#include "message/unread-message-repository.h"
#include "model/kadu-abstract-model.h"
#include "model/model-chain.h"

#include "talkable-delegate.h"

TalkableDelegate::TalkableDelegate(TalkableTreeView *parent) :
		KaduTreeViewDelegate(parent)
{
	connect(ContactManager::instance(), SIGNAL(contactUpdated(Contact)), this, SLOT(contactUpdated(Contact)));
	connect(BuddyPreferredManager::instance(), SIGNAL(buddyUpdated(Buddy)), this, SLOT(buddyUpdated(Buddy)));
	connect(Core::instance()->unreadMessageRepository(), SIGNAL(unreadMessageAdded(Message)), this, SLOT(messageStatusChanged(Message)));
	connect(Core::instance()->unreadMessageRepository(), SIGNAL(unreadMessageRemoved(Message)), this, SLOT(messageStatusChanged(Message)));
}

TalkableDelegate::~TalkableDelegate()
{
	disconnect(ContactManager::instance(), 0, this, 0);
	disconnect(BuddyPreferredManager::instance(), 0, this, 0);
	disconnect(Core::instance()->unreadMessageRepository(), 0, this, 0);
}

void TalkableDelegate::setChain(ModelChain *chain)
{
	Chain = chain;
}

void TalkableDelegate::contactUpdated(const Contact &contact)
{
	if (!Chain)
		return;

	const QModelIndexList &contactsIndexList = Chain.data()->indexListForValue(contact);
	foreach (const QModelIndex &contactIndex, contactsIndexList)
		emit sizeHintChanged(contactIndex);
}

void TalkableDelegate::buddyUpdated(const Buddy &buddy)
{
	if (!Chain)
		return;

	const QModelIndexList &buddyIndexList = Chain.data()->indexListForValue(buddy);
	foreach (const QModelIndex &buddyIndex, buddyIndexList)
		emit sizeHintChanged(buddyIndex);
}

void TalkableDelegate::messageStatusChanged(Message message)
{
	Buddy buddy = message.messageSender().ownerBuddy();
	buddyUpdated(buddy);
}

bool TalkableDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                   const QModelIndex &index)
{
	if (!event || !model)
		return false;

	Qt::ItemFlags flags = model->flags(index);
	if (!(flags & Qt::ItemIsUserCheckable) || !(option.state & QStyle::State_Enabled) || !(flags & Qt::ItemIsEnabled))
		return false;

	QVariant value = index.data(Qt::CheckStateRole);
	if (!value.isValid())
		return false;

	switch (event->type())
	{
		case QEvent::MouseButtonRelease:
		{
			TalkablePainter talkablePainter(configuration(), getOptions(index, option), index);

			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			if (Qt::LeftButton != mouseEvent->button())
				return false;

			const QRect &checkboxRect = talkablePainter.checkboxRect();
			if (!checkboxRect.contains(mouseEvent->pos()))
				return false;

			break;
		}
		case QEvent::MouseButtonDblClick:
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			if (Qt::LeftButton != mouseEvent->button())
				return false;

			break;
		}

		case QEvent::KeyPress:
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			if (Qt::Key_Space != keyEvent->key() && Qt::Key_Select != keyEvent->key())
				return false;

			break;
		}

		default:
			return false;
	}

	Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
	state = state == Qt::Checked ? Qt::Unchecked : Qt::Checked;

	return model->setData(index, state, Qt::CheckStateRole);
}

#include "moc_talkable-delegate.cpp"
