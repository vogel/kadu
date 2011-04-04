/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef KADU_ICON_H
#define KADU_ICON_H

#include <QtCore/QString>
#include <QtGui/QIcon>

#include "exports.h"

class KaduIcon;

class KaduIconThemeChangeWatcher : public QObject
{
	Q_OBJECT

	const KaduIcon *Icon;

private slots:
	void themeChanged() const;

public:
	explicit KaduIconThemeChangeWatcher(const KaduIcon *icon);
	virtual ~KaduIconThemeChangeWatcher();

};

class KADUAPI KaduIcon
{
	QString Path;
	QString IconSize;
	QString IconName;
	mutable QString FullPath;
	mutable QIcon Icon;

	KaduIconThemeChangeWatcher Watcher;

public:
	KaduIcon();
	explicit KaduIcon(const QString &path, const QString &size = QString(), const QString &name = QString());
	KaduIcon(const KaduIcon &copyMe);

	KaduIcon & operator = (const KaduIcon &copyMe);

	bool isNull() const;

	void clearCache() const;

	const QString & path() const { return Path; }
	// NOTE: it resets iconSize and iconName as well because new path may contain them
	// TODO: it could be avoided if fixed IconsManager
	void setPath(const QString &path);

	const QString & size() const { return IconSize; }
	void setSize(const QString &size);

	const QString & name() const { return IconName; }
	void setName(const QString &name);

	const QString & fullPath() const;
	QString webKitPath() const;
	const QIcon & icon() const;

};

#endif // KADU_ICON_H
