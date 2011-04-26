/*
 * %kadu copyright begin%
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

#ifndef DESKTOP_DOCK_WINDOW_H
#define DESKTOP_DOCK_WINDOW_H

#include <QtGui/QLabel>

#include "configuration/configuration-aware-object.h"

class DesktopDockWindow : public QLabel, ConfigurationAwareObject
{
	Q_OBJECT

	bool IsMoving;

	QPoint getCenterFromEvent(QMouseEvent *ev);

protected:
    virtual void contextMenuEvent(QContextMenuEvent *ev);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);

	virtual void configurationUpdated();

public:
	explicit DesktopDockWindow(QWidget *parent = 0);
	virtual ~DesktopDockWindow();

public slots:
	void startMoving();

signals:
	void dropped(const QPoint &);

};


#endif // DESKTOP_DOCK_WINDOW_H
