/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "gui/widgets/talkable-delegate-configuration.h"
#include "model/roles.h"

#include "avatar-painter.h"

AvatarPainter::AvatarPainter(const TalkableDelegateConfiguration &configuration, const QStyleOptionViewItemV4 &option, const QRect &avatarRect, const QModelIndex &index) :
		Configuration(configuration), Option(option), AvatarRect(avatarRect), Index(index)
{
	Avatar = Index.data(AvatarRole).value<QPixmap>();
}

bool AvatarPainter::greyOut()
{
	if (!Configuration.avatarGreyOut())
		return false;

	Contact contact = Index.data(ContactRole).value<Contact>();
	return contact.currentStatus().isDisconnected();
}

QString AvatarPainter::cacheKey()
{
	return QString("msi-%1-%2,%3,%4")
			.arg(Avatar.cacheKey())
			.arg(greyOut())
			.arg(Configuration.avatarBorder())
			.arg(Option.state & QStyle::State_Selected ? 1 : 0);
}

QPixmap AvatarPainter::getOrCreateCacheItem()
{
	QString key = cacheKey();

	QPixmap cached;
	if (QPixmapCache::find(key, &cached))
		return cached;

	QPixmap item = QPixmap(AvatarRect.size());
	item.fill(QColor(0, 0, 0, 0));

	QPainter cachePainter;
	cachePainter.begin(&item);
	doPaint(&cachePainter, item.size());
	cachePainter.end();

	QPixmapCache::insert(key, item);

	return item;
}

void AvatarPainter::paintFromCache(QPainter *painter)
{
	QPixmap cached = getOrCreateCacheItem();

	painter->drawPixmap(AvatarRect, cached);
}

void AvatarPainter::doPaint(QPainter *painter, const QSize &size)
{
	QPixmap displayAvatar;

	if (Avatar.size() != size)
		displayAvatar = Avatar.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	else
		displayAvatar = Avatar;

	QRect displayRect = displayAvatar.rect();
	displayRect.moveTop(0);
	displayRect.moveLeft((size.width() - displayRect.width()) / 2);

	// grey out offline contacts' avatar
	if (greyOut())
		painter->drawPixmap(displayRect, QIcon(displayAvatar).pixmap(displayAvatar.size(), QIcon::Disabled));
	else
		painter->drawPixmap(displayRect, displayAvatar);

	// draw avatar border
	if (Configuration.avatarBorder())
		painter->drawRect(displayRect.adjusted(0, 0, -1, -1));
}

void AvatarPainter::paint(QPainter *painter)
{
	if (!Configuration.showAvatars() || AvatarRect.isEmpty() || Avatar.isNull())
		return;

	paintFromCache(painter);
}
