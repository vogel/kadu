/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef TITLE_WIDGET_H
#define TITLE_WIDGET_H

#include <QtWidgets/QWidget>

class QLabel;

class TitleWidget : public QWidget
{
	Q_OBJECT

	QString text;
	QString comment;
	QString pixmap;

	QLabel *commentLabel;
	QLabel *textLabel;
	
public:
	explicit TitleWidget(QWidget *parent);
	virtual ~TitleWidget();

	void setText(const QString &text, Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter);
	void setComment(const QString &comment);
};

#endif // TITLE_WIDGET_H
