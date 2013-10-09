/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "themes/theme.h"
#include "exports.h"

class KADUAPI ThemeManager : public QObject
{
	Q_OBJECT

	QMap<QString, Theme> Themes;
	QString CurrentThemeName;

protected:
	QStringList getSubDirs(const QString &dirPath) const;

	virtual QString defaultThemeName() const = 0;
	virtual QStringList defaultThemePaths() const = 0;
	virtual bool isValidThemePath(const QString &themePath) const = 0;

public:
	explicit ThemeManager(QObject *parent = 0);
	virtual ~ThemeManager();

	void loadThemes();

	const QMap<QString, Theme> & themes() const { return Themes; }

	void setCurrentTheme(const QString &themeName);
	Theme currentTheme() const;

signals:
	void themeListUpdated();

};

#endif // THEME_MANAGER_H
