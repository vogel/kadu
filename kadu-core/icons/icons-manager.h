/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QHash>
#include <QtGui/QIcon>

#include "configuration/configuration-aware-object.h"
#include "themes/theme.h"
#include "exports.h"

class IconThemeManager;

class KADUAPI IconsManager : public QObject, public ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(IconsManager)

	IconsManager();

	static IconsManager *Instance;

	IconThemeManager *ThemeManager;

	QHash<QString, QIcon> IconCache;

	void clearCache();

	QString localProtocolPath;

	QIcon buildPngIcon(const QString &path);
	QIcon buildSvgIcon(const QString &path);

protected:
	virtual void configurationUpdated();

public:
	static IconsManager * instance();

	IconThemeManager * themeManager() const;

	QString iconPathAllowEmpty(const QString &path, const QString &size, const QString &name) const;
	QString iconPathAllowEmpty(const QString &path, const QString &size) const;
	QString iconPathAllowEmpty(const QString &path) const;

	QString iconPath(const QString &path, const QString &size, const QString &name) const;
	QString iconPath(const QString &path, const QString &size) const;
	QString iconPath(const QString &path) const;

	const QIcon & iconByPath(const QString &path, bool allowEmpty = false);

	QSize getIconsSize();

signals:
	void themeChanged();

};

#endif //ICONS_MANAGER_H
