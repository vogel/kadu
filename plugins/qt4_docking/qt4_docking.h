/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
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
#include <QtGui/QSystemTrayIcon>

#include "plugins/docking/docker.h"

#include "qt4_docking_exports.h"

class QMovie;

class QT4DOCKAPI Qt4TrayIcon : public QSystemTrayIcon, public Docker
{
	Q_OBJECT

	QMovie *Movie;
	QPoint lastPosition;

	static Qt4TrayIcon *Instance;

	explicit Qt4TrayIcon(QWidget *parent = 0);
	virtual ~Qt4TrayIcon();

private slots:
	void trayActivated(QSystemTrayIcon::ActivationReason reason);

	void movieUpdate();

public:
	static Qt4TrayIcon * instance();

	static Qt4TrayIcon * createInstance();
	static void destroyInstance();

	virtual void changeTrayIcon(const KaduIcon &icon);
	virtual void changeTrayMovie(const QString &moviePath);
	virtual void changeTrayTooltip(const QString &tooltip);
	virtual QPoint trayPosition();
};


#endif // QT4_DOCKING_H
