/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef LINE_EDIT_CLEAR_BUTTON_H
#define LINE_EDIT_CLEAR_BUTTON_H

#include <QtGui/QIcon>
#include <QtGui/QWidget>

class QTimeLine;

class LineEditClearButton : public QWidget
{
	Q_OBJECT

	QTimeLine *Timeline;
	QPixmap ButtonPixmap;
	QIcon ButtonIcon;

	void setUpTimeLine();

protected:
	void paintEvent(QPaintEvent *event);
	virtual bool event(QEvent *event);

protected slots:
	void animationFinished();

public:
	explicit LineEditClearButton(QWidget *parent);
	virtual ~LineEditClearButton();

	void setPixmap(const QPixmap &buttonPixmap);
	const QPixmap & pixmap() const { return ButtonPixmap; }

	virtual QSize sizeHint() const { return ButtonPixmap.size(); }

	void setAnimationsEnabled(bool animationsEnabled);
	void animateVisible(bool visible);

};

#endif // LINE_EDIT_CLEAR_BUTTON_H
