/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef TALKABLE_PAINTER_H
#define TALKABLE_PAINTER_H

#include <QtGui/QStyleOption>

class QTextDocument;
class QTreeView;

class TalkableDelegateConfiguration;

class TalkablePainter
{
	const TalkableDelegateConfiguration &Configuration;
	QStyleOptionViewItemV4 Option;
	const QModelIndex &Index;
	const QTreeView *Widget;
	const QStyle *Style;

	int HFrameMargin;
	int VFrameMargin;

	QFontMetrics FontMetrics;
	QFontMetrics BoldFontMetrics;
	QFontMetrics DescriptionFontMetrics;

	QTextDocument *DescriptionDocument;

	QRect ItemRect;

	QRect CheckboxRect;
	QRect IconRect;
	QRect AvatarRect;
	QRect IdentityNameRect;
	QRect NameRect;
	QRect DescriptionRect;

	QString getIdentityName();
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
	bool showCheckbox() const;
	bool showMessagePixmap() const;
	bool showIdentityName() const;
	bool showDescription() const;

	void computeCheckboxRect();
	void computeIconRect();
	void computeAvatarRect();
	void computeIdentityNameRect();
	void computeNameRect();
	void computeDescriptionRect();

	void computeLayout();

	void paintDebugRect(QPainter *painter, QRect rect, QColor color) const;

	void paintCheckbox(QPainter *painter);
	void paintIcon(QPainter *painter);
	void paintAvatar(QPainter *painter);
	void paintIdentityName(QPainter *painter);
	void paintName(QPainter *painter);
	void paintDescription(QPainter *painter);

#ifdef Q_OS_WIN32
	static bool useColorsWorkaround();
#endif

public:
	TalkablePainter(const TalkableDelegateConfiguration &configuration, QStyleOptionViewItemV4 option, const QModelIndex &index);
	~TalkablePainter();

	QPixmap avatar() const;
	QPixmap icon() const;

	int height();
	void paint(QPainter *painter);
	QRect checkboxRect();

};

#endif // TALKABLE_PAINTER_H
