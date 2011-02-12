/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef QT4_DOCKING_H
#define QT4_DOCKING_H

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtGui/QIcon>
#include <QtGui/QSystemTrayIcon>

#include "modules/docking/docker.h"

#include "qt4_docking_exports.h"

class QMovie;

class QT4DOCKAPI Qt4TrayIcon : public QSystemTrayIcon, public Docker
{
	Q_OBJECT

	QMovie *Movie;
	QPoint lastPosition;

private slots:
	void trayActivated(QSystemTrayIcon::ActivationReason reason);

	void movieUpdate();

public:
	explicit Qt4TrayIcon(QWidget *parent = 0);
	virtual ~Qt4TrayIcon();

	virtual void changeTrayIcon(const QIcon &icon);
	virtual void changeTrayMovie(const QString &moviePath);
	virtual void changeTrayTooltip(const QString &tooltip);
	virtual QPoint trayPosition();
};

extern QT4DOCKAPI Qt4TrayIcon *qt4_tray_icon;

#endif // QT4_DOCKING_H
