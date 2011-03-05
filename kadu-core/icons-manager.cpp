/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
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

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
#include "themes/icon-theme-manager.h"
#include "debug.h"

#include "icons-manager.h"

IconsManager * IconsManager::Instance = 0;

IconsManager * IconsManager::instance()
{
	if (Instance == 0)
		Instance = new IconsManager();
	return Instance;
}

IconsManager::IconsManager()
{
	kdebugf();

	QStringList iconPaths = config_file.readEntry("Look", "IconsPaths").split(QRegExp("[;:&]"), QString::SkipEmptyParts);

	ThemeManager = new IconThemeManager(this);
	ThemeManager->loadThemes(iconPaths);
	ThemeManager->setCurrentTheme(config_file.readEntry("Look", "IconTheme"));
	configurationUpdated();

	config_file.writeEntry("Look", "IconTheme", ThemeManager->currentTheme().path());

	// TODO: localized protocol
	localProtocolPath = "gadu-gadu";

	kdebugf2();
}

IconThemeManager * IconsManager::themeManager() const
{
	return ThemeManager;
}

QString IconsManager::iconPathAllowEmpty(const QString &path, const QString &size, const QString &name) const
{
	QFileInfo fileInfo;

	fileInfo.setFile( ThemeManager->currentTheme().path() + path + '/' + size + '/' + name + ".png" );
	if (fileInfo.isFile() && fileInfo.isReadable())
		return fileInfo.canonicalFilePath();

	fileInfo.setFile( ThemeManager->currentTheme().path() + path + "/svg/" + name + ".svg" );
	if (fileInfo.isFile() && fileInfo.isReadable())
		return fileInfo.canonicalFilePath();

	fileInfo.setFile( ThemeManager->currentTheme().path() + path + "/svg/" + name + ".svgz" );
	if (fileInfo.isFile() && fileInfo.isReadable())
		return fileInfo.canonicalFilePath();

	if (path == "protocols/common")
	{
		QString protocolpath;
		if (AccountManager::instance()->defaultAccount())
			protocolpath = AccountManager::instance()->defaultAccount().protocolHandler()->statusPixmapPath();
		else
			protocolpath = localProtocolPath;
		QString path2 = QString("protocols/%1").arg(protocolpath);
		return iconPathAllowEmpty(path2, size, name);
	}

	return QString();
}

QString IconsManager::iconPathAllowEmpty(const QString &path, const QString &size) const
{
	QString realPath;
	QString iconName;

	int lastHash = path.lastIndexOf('/');
	if (-1 != lastHash)
	{
		realPath = path.left(lastHash);
		iconName = path.mid(lastHash + 1);
	}
	else
		iconName = path;

	return iconPathAllowEmpty(realPath, size, iconName);
}

QString IconsManager::iconPathAllowEmpty(const QString &path) const
{
	QString fileName = ThemeManager->currentTheme().path() + path;

	QFileInfo fileInfo(fileName);
	if (!fileInfo.isFile() || !fileInfo.isReadable())
		return QString();

	return fileInfo.canonicalFilePath();
}

QString IconsManager::iconPath(const QString &path, const QString &size, const QString &name) const
{
	QString result = iconPathAllowEmpty(path, size, name);
	if (!result.isEmpty())
		return result;
	return iconPathAllowEmpty("kadu_icons", size, "0");
}

QString IconsManager::iconPath(const QString &path, const QString &size) const
{
	QString result = iconPathAllowEmpty(path, size);
	if (!result.isEmpty())
		return result;
	return iconPathAllowEmpty("kadu_icons/0", size);
}

QString IconsManager::iconPath(const QString &path) const
{
	QString result = iconPathAllowEmpty(path);
	if (!result.isEmpty())
		return result;
	return iconPathAllowEmpty("kadu_icons/64x64/0");
}

QIcon IconsManager::buildPngIcon(const QString &path)
{
	static QLatin1String sizes [] = {
		QLatin1String("16x16"),
		QLatin1String("22x22"),
		QLatin1String("32x32"),
		QLatin1String("64x64"),
		QLatin1String("128x128")
	};
	static int sizes_count = 5;

	QIcon icon;
	for (int i = 0; i < sizes_count; i++)
	{
		QString fullPath = iconPathAllowEmpty(path, sizes[i]);
		if (!fullPath.isEmpty())
			icon.addFile(fullPath);
	}

	return icon;
}

QIcon IconsManager::buildSvgIcon(const QString& path)
{
	QIcon icon;
	QString realPath;
	QString iconName;

	int lastHash = path.lastIndexOf('/');
	if (lastHash != -1)
	{
		realPath = path.left(lastHash);
		iconName = path.mid(lastHash + 1);
	}
	else
		iconName = path;

	QFileInfo fileInfo;

	fileInfo.setFile( ThemeManager->currentTheme().path() + realPath + "/svg/" + iconName + ".svg" );
	if (fileInfo.isFile() && fileInfo.isReadable())
		icon.addFile(fileInfo.canonicalFilePath());

	fileInfo.setFile( ThemeManager->currentTheme().path() + realPath + "/svg/" + iconName + ".svgz" );
	if (fileInfo.isFile() && fileInfo.isReadable())
		icon.addFile(fileInfo.canonicalFilePath());

	return icon;
}

const QIcon & IconsManager::iconByPath(const QString &path, bool allowEmpty)
{
	if (!IconCache.contains(path))
	{
		QIcon icon;

		QFileInfo fileInfo(path);
		if (fileInfo.isAbsolute() && fileInfo.isReadable())
			icon.addFile(path);
		else
		{
			icon = buildSvgIcon(path);

			if (icon.isNull())
				icon = buildPngIcon(path);

			if (icon.isNull())
			{
				QRegExp commonRegexp = QRegExp("^protocols/common/(.+)$");
				if (path.contains(commonRegexp))
				{
					QString protocolpath;
					if (AccountManager::instance()->defaultAccount())
						protocolpath = AccountManager::instance()->defaultAccount().protocolHandler()->statusPixmapPath();
					else
						protocolpath = localProtocolPath;
					QString path2 = QString("protocols/%1/%2").arg(protocolpath, commonRegexp.cap(1));
					return iconByPath(path2);
				}
			}

			if (icon.isNull() && !allowEmpty)
				icon = buildSvgIcon("kadu_icons/0");

			if (icon.isNull() && !allowEmpty)
				icon = buildPngIcon("kadu_icons/0");

		}

		IconCache.insert(path, icon);
	}

	return IconCache[path];
}

void IconsManager::clearCache()
{
	IconCache.clear();
}

void IconsManager::configurationUpdated()
{
	bool themeWasChanged = config_file.readEntry("Look", "IconTheme") != ThemeManager->currentTheme().path();
	if (themeWasChanged)
	{
		clearCache();
		ThemeManager->setCurrentTheme(config_file.readEntry("Look", "IconTheme"));
		config_file.writeEntry("Look", "IconTheme", ThemeManager->currentTheme().path());

		emit themeChanged();
	}
}

QSize IconsManager::getIconsSize()
{
	return QSize(16, 16);
}
