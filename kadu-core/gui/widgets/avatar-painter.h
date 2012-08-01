/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef AVATAR_PAINTER_H
#define AVATAR_PAINTER_H

#include <QtCore/QString>
#include <QtGui/QPixmap>

class QModelIndex;
class QStyleOptionViewItemV4;

class TalkableDelegateConfiguration;

class AvatarPainter
{
	const TalkableDelegateConfiguration &Configuration;
	const QStyleOptionViewItemV4 &Option;
	const QRect &AvatarRect;
	const QModelIndex &Index;

	QPixmap Avatar;

	bool greyOut();

	QString cacheKey();
	QPixmap getOrCreateCacheItem();
	void paintFromCache(QPainter *painter);

	void doPaint(QPainter *painter, const QSize &size);

public:
	AvatarPainter(const TalkableDelegateConfiguration &configuration, const QStyleOptionViewItemV4 &option, const QRect &avatarRect, const QModelIndex &index);

	void paint(QPainter *painter);

};

#endif // AVATAR_PAINTER_H
