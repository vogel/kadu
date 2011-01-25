/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDIES_LIST_VIEW_AVATAR_PAINTER_H
#define BUDDIES_LIST_VIEW_AVATAR_PAINTER_H

#include <QtCore/QString>
#include <QtGui/QPixmap>

class QModelIndex;
class QStyleOptionViewItemV4;

class BuddiesListViewDelegateConfiguration;

class BuddiesListViewAvatarPainter
{
	const BuddiesListViewDelegateConfiguration &Configuration;
	const QStyleOptionViewItemV4 &Option;
	const QRect &AvatarRect;
	const QModelIndex &Index;

	QPixmap Avatar;

	bool greyOut();

	QString cacheKey();
	void createCacheItem();
	void paintFromCache(QPainter *painter);

	void doPaint(QPainter *painter, const QRect &rect);

public:
	BuddiesListViewAvatarPainter(const BuddiesListViewDelegateConfiguration &configuration, const QStyleOptionViewItemV4 &option, const QRect &avatarRect, const QModelIndex &index);

	void paint(QPainter *painter);

};

#endif // BUDDIES_LIST_VIEW_AVATAR_PAINTER_H
