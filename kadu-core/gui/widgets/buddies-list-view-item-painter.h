/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

class BuddiesListViewDelegateConfiguration;

class BuddiesListViewItemPainter
{
	const BuddiesListViewDelegateConfiguration &Configuration;
	QStyleOptionViewItemV4 Option;
	const QModelIndex &Index;
	const QTreeView *Widget;

	bool UseConfigurationColors;

	int HFrameMargin;
	int VFrameMargin;

	QFontMetrics FontMetrics;
	QFontMetrics BoldFontMetrics;
	QFontMetrics DescriptionFontMetrics;

	QTextDocument *DescriptionDocument;

	QRect ItemRect;

	QRect IconRect;
	QRect MessageIconRect;
	QRect AvatarRect;
	QRect AccountNameRect;
	QRect NameRect;
	QRect DescriptionRect;

	QString getAccountName();
	QString getName();

	bool drawSelected() const;
	bool drawDisabled() const;

	QTextDocument * createDescriptionDocument(const QString &text, int width, QColor color) const;
	QTextDocument * getDescriptionDocument(int width);

	const QFontMetrics & fontMetrics();
	int itemIndentation();

	void fixColors();
	QColor textColor() const;

	bool useBold() const;
	bool showMessagePixmap() const;
	bool showAccountName() const;
	bool showDescription() const;

	void computeIconRect();
	void computeMessageIconRect();
	void computeAvatarRect();
	void computeAccountNameRect();
	void computeNameRect();
	void computeDescriptionRect();

	void computeLayout();

	void paintDebugRect(QPainter *painter, QRect rect, QColor color) const;

	void paintIcon(QPainter *painter);
	void paintMessageIcon(QPainter *painter);
	void paintAvatar(QPainter *painter);
	void paintAccountName(QPainter *painter);
	void paintName(QPainter *painter);
	void paintDescription(QPainter *painter);

public:
	BuddiesListViewItemPainter(const BuddiesListViewDelegateConfiguration &configuration, QStyleOptionViewItemV4 option, const QModelIndex &index, bool useConfigurationColors);
	~BuddiesListViewItemPainter();

	QPixmap buddyAvatar() const;
	QPixmap buddyIcon() const;

	int height();
	void paint(QPainter *painter);

};

#endif // BUDDIES_LIST_VIEW_ITEM_PAINTER_H
