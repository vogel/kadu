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
#include "avatars/avatar.h"
#include "avatars/avatar-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/model/abstract-buddies-model.h"
#include "chat/message/pending-messages-manager.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/buddies-list-view-item-painter.h"

#include "buddies-list-view-delegate.h"

BuddiesListViewDelegate::BuddiesListViewDelegate(QObject *parent) :
		QItemDelegate(parent), Model(0), Configuration(parent), UseConfigurationColors(false)
{
	connect(AvatarManager::instance(), SIGNAL(avatarUpdated(Avatar)), this, SLOT(avatarUpdated(Avatar)));
	connect(ContactManager::instance(), SIGNAL(contactUpdated(Contact&)), this, SLOT(contactUpdated(Contact&)));
	connect(BuddyPreferredManager::instance(), SIGNAL(buddyUpdated(Buddy&)), this, SLOT(buddyUpdated(Buddy&)));
	connect(PendingMessagesManager::instance(), SIGNAL(messageAdded(Message)), this, SLOT(messageStatusChanged(Message)));
	connect(PendingMessagesManager::instance(), SIGNAL(messageRemoved(Message)), this, SLOT(messageStatusChanged(Message)));
}

BuddiesListViewDelegate::~BuddiesListViewDelegate()
{
	disconnect(AvatarManager::instance(), SIGNAL(avatarUpdated(Avatar)), this, SLOT(avatarUpdated(Avatar)));
	disconnect(ContactManager::instance(), SIGNAL(contactUpdated(Contact&)), this, SLOT(contactUpdated(Contact&)));
	disconnect(BuddyPreferredManager::instance(), SIGNAL(buddyUpdated(Buddy&)), this, SLOT(buddyUpdated(Buddy&)));
	disconnect(PendingMessagesManager::instance(), SIGNAL(messageAdded(Message)), this, SLOT(messageStatusChanged(Message)));
	disconnect(PendingMessagesManager::instance(), SIGNAL(messageRemoved(Message)), this, SLOT(messageStatusChanged(Message)));
}

void BuddiesListViewDelegate::setModel(AbstractBuddiesModel *model)
{
	Model = model;
	QAbstractItemModel *itemModel = dynamic_cast<QAbstractItemModel *>(Model);
	if (itemModel)
		connect(itemModel, SIGNAL(destroyed(QObject *)), this, SLOT(modelDestroyed()));
}

void BuddiesListViewDelegate::avatarUpdated(Avatar avatar)
{
	if (!Model)
		return;

	if (avatar.avatarContact())
	{
		Buddy buddy = avatar.avatarContact().ownerBuddy();
		Contact contact = avatar.avatarContact();

		QModelIndex buddyIndex = Model->indexForValue(buddy);
		QModelIndex contactIndex = buddyIndex.child(buddy.contacts().indexOf(contact), 0);
		emit sizeHintChanged(buddyIndex);
		emit sizeHintChanged(contactIndex);
	}
	else if (avatar.avatarBuddy())
		emit sizeHintChanged(Model->indexForValue(avatar.avatarBuddy()));
}

void BuddiesListViewDelegate::contactUpdated(Contact &contact)
{
	if (Model)
		emit sizeHintChanged(Model->indexForValue(contact.ownerBuddy()));
}

void BuddiesListViewDelegate::buddyUpdated(Buddy &buddy)
{
	if (Model)
		emit sizeHintChanged(Model->indexForValue(buddy));
}

void BuddiesListViewDelegate::messageStatusChanged(Message message)
{
	Buddy buddy = message.messageSender().ownerBuddy();
	buddyUpdated(buddy);
}

void BuddiesListViewDelegate::modelDestroyed()
{
	Model = 0;
}

void BuddiesListViewDelegate::setShowAccountName(bool showAccountName)
{
	Configuration.setShowAccountName(showAccountName);
}

QStyleOptionViewItemV4 BuddiesListViewDelegate::getOptions(const QModelIndex &index, const QStyleOptionViewItem &option) const
{
	QStyleOptionViewItemV4 opt = setOptions(index, option);

	const QStyleOptionViewItemV2 *v2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>(&option);
	opt.features = v2
		? v2->features
		: QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);
	const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
	opt.locale = v3 ? v3->locale : QLocale();
	opt.widget = v3 ? v3->widget : 0;

	return opt;
}

QSize BuddiesListViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	BuddiesListViewItemPainter buddyPainter(Configuration, getOptions(index, option), index, UseConfigurationColors);
	return QSize(0, buddyPainter.height());
}

void BuddiesListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 options = getOptions(index, option);

	const QAbstractItemView *widget = qobject_cast<const QAbstractItemView *>(options.widget);
	if (!widget)
		return;

	QStyle *style = widget->style();
	style->drawControl(QStyle::CE_ItemViewItem, &options, painter, widget);

	BuddiesListViewItemPainter buddyPainter(Configuration, options, index, UseConfigurationColors);
	buddyPainter.paint(painter);
}
