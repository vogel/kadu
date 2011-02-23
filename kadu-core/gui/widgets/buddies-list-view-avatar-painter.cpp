/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 * Copyright 2010 Dariusz Markowicz
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

#include <QtCore/QModelIndex>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QPixmapCache>
#include <QtGui/QStyleOptionViewItem>

#include "contacts/contact.h"
#include "gui/widgets/buddies-list-view-delegate-configuration.h"
#include "model/roles.h"

#include "buddies-list-view-avatar-painter.h"

BuddiesListViewAvatarPainter::BuddiesListViewAvatarPainter(const BuddiesListViewDelegateConfiguration &configuration, const QStyleOptionViewItemV4 &option, const QRect &avatarRect, const QModelIndex &index) :
		Configuration(configuration), Option(option), AvatarRect(avatarRect), Index(index)
{
	Avatar = Index.data(AvatarRole).value<QPixmap>();
}

bool BuddiesListViewAvatarPainter::greyOut()
{
	if (!Configuration.avatarGreyOut())
		return false;

	Contact contact = Index.data(ContactRole).value<Contact>();
	return contact.currentStatus().isDisconnected();
}

QString BuddiesListViewAvatarPainter::cacheKey()
{
	return QString("msi-%1-%2,%3,%4")
			.arg(Avatar.cacheKey())
			.arg(greyOut())
			.arg(Configuration.avatarBorder())
			.arg(Option.state & QStyle::State_Selected ? 1 : 0);
}

void BuddiesListViewAvatarPainter::createCacheItem()
{
	QPixmap item = QPixmap(AvatarRect.size());
	item.fill(QColor(0, 0, 0, 0));

	QRect rect = QRect(0, 0, AvatarRect.width() - 1, AvatarRect.height() - 1);
	QPainter cachePainter;
	cachePainter.begin(&item);
	doPaint(&cachePainter, rect);
	cachePainter.end();

	QPixmapCache::insert(cacheKey(), item);
}

void BuddiesListViewAvatarPainter::paintFromCache(QPainter *painter)
{
	QString key = cacheKey();
	QPixmap cached;

	if (!QPixmapCache::find(key, &cached))
	{
		createCacheItem();
		if (!QPixmapCache::find(key, &cached))
			return; // cache error;
	}

	painter->drawPixmap(AvatarRect, cached);
}

void BuddiesListViewAvatarPainter::doPaint(QPainter *painter, const QRect &rect)
{
	QPixmap displayAvatar;

	if (Avatar.size() != rect.size())
		displayAvatar = Avatar.scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	else
		displayAvatar = Avatar;

	QRect displayRect = displayAvatar.rect();
	displayRect.moveTop(rect.top());
	displayRect.moveLeft(rect.left() + (rect.width() - displayRect.width()) / 2);

	// grey out offline contacts' avatar
	if (greyOut())
		painter->drawPixmap(displayRect, QIcon(displayAvatar).pixmap(displayAvatar.size(), QIcon::Disabled));
	else
		painter->drawPixmap(displayRect, displayAvatar);

	// draw avatar border
	if (Configuration.avatarBorder())
		painter->drawRect(displayRect);
}

void BuddiesListViewAvatarPainter::paint(QPainter *painter)
{
	if (!Configuration.showAvatars() || AvatarRect.isEmpty() || Avatar.isNull())
		return;

	paintFromCache(painter);
}
