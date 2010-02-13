/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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

#ifndef ICONS_MANAGER_H
#define ICONS_MANAGER_H

#include <QtCore/QMap>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>

#include "configuration/configuration-aware-object.h"

#include "themes.h"
#include "exports.h"

class KADUAPI IconsManager : public Themes, ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(IconsManager)

	IconsManager();

	static IconsManager *Instance;

	QMap<QString, QPixmap> pixmaps;
	QMap<QString, QIcon> icons;

public:
	QString iconPath(const QString &name) const;
	
	const QPixmap & pixmapByPath(const QString &name);
	const QIcon & iconByPath(const QString &name);

	QSize getIconsSize();

	static IconsManager * instance();	

public: // TODO: fix, see Kadu::Kadu
	virtual void configurationUpdated();

public slots:
	void clear();

signals:
	void themeChanged();

};

#endif //ICONS_MANAGER_H
