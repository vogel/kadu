/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2008 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
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

#ifndef SELECT_FONT_H
#define SELECT_FONT_H

#include <QtGui/QWidget>

#include "exports.h"

class QLineEdit;

class KADUAPI SelectFont : public QWidget
{
	Q_OBJECT

	QFont currentFont;
	QLineEdit *fontEdit;

private slots:
	void onClick();

public:
	SelectFont(QWidget *parent = 0);
	const QFont &font() const;
	void setFont(const QFont &font);

signals:
	void fontChanged(QFont font);

};

#endif // SELECT_FONT_H
