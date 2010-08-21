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

#ifndef BUDDIES_LIST_VIEW_ITEM_PAINTER_H
#define BUDDIES_LIST_VIEW_ITEM_PAINTER_H

#include <QtGui/QStyleOption>

class QTextDocument;
class QTreeView;

class BuddiesListViewDelegate;
class BuddiesListViewDelegateConfiguration;

class BuddiesListViewItemPainter
{
	const BuddiesListViewDelegateConfiguration &Configuration;
	const BuddiesListViewDelegate *Delegate;
	const QStyleOptionViewItemV4 &Option;
	const QModelIndex &Index;
	const QTreeView *Widget;

public:
	BuddiesListViewItemPainter(const BuddiesListViewDelegateConfiguration &configuration, const BuddiesListViewDelegate *delegate, const QStyleOptionViewItemV4 &option, const QModelIndex &index);

	QSize sizeHint();

	QTextDocument * descriptionDocument(const QString &text, int width, QColor color) const;
	int textAvailableWidth(const QTreeView *widget) const;
	int iconsWidth(int margin) const;

	QPixmap buddyAvatar() const;
	QPixmap buddyIcon() const;

	int buddyIconHeight() const;
	int buddyAvatarHeight() const;
	int buddyNameHeight() const;
	int buddyDescriptionHeight(int availableWidth) const;
	int buddyHeight() const;

};

#endif // BUDDIES_LIST_VIEW_ITEM_PAINTER_H
