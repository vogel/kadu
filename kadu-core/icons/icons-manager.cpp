/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "icons/kadu-icon.h"
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

	ThemeManager = new IconThemeManager(this);
	ThemeManager->loadThemes();
	ThemeManager->setCurrentTheme(Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "IconTheme"));
	configurationUpdated();

	Application::instance()->configuration()->deprecatedApi()->writeEntry("Look", "IconTheme", ThemeManager->currentTheme().name());

	// TODO: localized protocol
	localProtocolPath = "gadu-gadu";

	kdebugf2();
}

IconThemeManager * IconsManager::themeManager() const
{
	return ThemeManager;
}

QString IconsManager::iconPath(const KaduIcon &icon, IconsManager::AllowEmpty allowEmpty) const
{
	QString path = icon.path();
	QString size = icon.size();

	QFileInfo fileInfo(path);
	QString themePath = icon.themePath().isEmpty() ? ThemeManager->currentTheme().path() : icon.themePath();
	QString name = fileInfo.fileName();
	QString realPath = fileInfo.path();

	fileInfo.setFile(themePath + realPath + '/' + size + '/' + name + ".png" );
	if (fileInfo.isFile() && fileInfo.isReadable())
		return fileInfo.canonicalFilePath();

	fileInfo.setFile(themePath + realPath + '/' + size + '/' + name + ".gif" );
	if (fileInfo.isFile() && fileInfo.isReadable())
		return fileInfo.canonicalFilePath();

	fileInfo.setFile(themePath + realPath + "/svg/" + name + ".svg" );
	if (fileInfo.isFile() && fileInfo.isReadable())
		return fileInfo.canonicalFilePath();

	fileInfo.setFile(themePath + realPath + "/svg/" + name + ".svgz" );
	if (fileInfo.isFile() && fileInfo.isReadable())
		return fileInfo.canonicalFilePath();

	if (realPath == QLatin1String("protocols/common"))
	{
		QString protocolPath;
		if (AccountManager::instance()->defaultAccount().protocolHandler())
			protocolPath = AccountManager::instance()->defaultAccount().protocolHandler()->statusPixmapPath();
		else
			protocolPath = localProtocolPath;

		KaduIcon protocolPathIcon = icon;
		protocolPathIcon.setPath(QString("protocols/%1/%2").arg(protocolPath).arg(name));
		return iconPath(protocolPathIcon, allowEmpty);
	}

	if (EmptyAllowed == allowEmpty)
		return QString();
	else
		return iconPath(KaduIcon("kadu_icons/0", size), EmptyAllowed);
}

QIcon IconsManager::buildPngIcon(const QString &themePath, const QString &path)
{
	static QLatin1String sizes [] = {
		QLatin1String("16x16"),
		QLatin1String("22x22"),
		QLatin1String("32x32"),
		QLatin1String("64x64"),
		QLatin1String("96x96"),
		QLatin1String("128x128"),
		QLatin1String("256x256")
	};
	static int sizes_count = 7;

	QIcon icon;
	for (int i = 0; i < sizes_count; i++)
	{
		KaduIcon kaduIcon(path, sizes[i]);
		kaduIcon.setThemePath(themePath);

		QString fullPath = iconPath(kaduIcon, EmptyAllowed);
		if (!fullPath.isEmpty())
			icon.addFile(fullPath);
	}

	return icon;
}

QIcon IconsManager::buildSvgIcon(const QString &themePath, const QString& path)
{
	Q_UNUSED(themePath);
	Q_UNUSED(path);

	return {};
/*
	QIcon icon;
	QString theme = themePath.isEmpty() ? ThemeManager->currentTheme().path() : themePath;
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
	fileInfo.setFile(theme + realPath + "/svg/" + iconName + ".svgz");
	if (fileInfo.isFile() && fileInfo.isReadable())
	{
		icon.addFile(fileInfo.canonicalFilePath());
	}
	else
	{
		fileInfo.setFile(theme + realPath + "/svg/" + iconName + ".svg" );
		if (fileInfo.isFile() && fileInfo.isReadable())
		{
			icon.addFile(fileInfo.canonicalFilePath());
		}
	}

	return icon;
*/
}

QIcon IconsManager::iconByPath(const QString &themePath, const QString &path, AllowEmpty allowEmpty)
{
	if (!IconCache.contains(themePath + path))
	{
		QIcon icon;

		QFileInfo fileInfo(path);
		if (fileInfo.isAbsolute() && fileInfo.isReadable())
			icon.addFile(path);
		else
		{
			icon = buildSvgIcon(themePath, path);

			if (icon.isNull())
				icon = buildPngIcon(themePath, path);

			if (icon.isNull())
			{
				QRegExp commonRegexp = QRegExp("^protocols/common/(.+)$");
				if (path.contains(commonRegexp))
				{
					QString protocolpath;
					if (AccountManager::instance()->defaultAccount().protocolHandler())
						protocolpath = AccountManager::instance()->defaultAccount().protocolHandler()->statusPixmapPath();
					else
						protocolpath = localProtocolPath;
					return iconByPath(themePath, QString("protocols/%1/%2").arg(protocolpath, commonRegexp.cap(1)));
				}
			}

			if (icon.isNull() && EmptyNotAllowed == allowEmpty)
				icon = buildSvgIcon(themePath, "kadu_icons/0");

			if (icon.isNull() && EmptyNotAllowed == allowEmpty)
				icon = buildPngIcon(themePath, "kadu_icons/0");

		}

		IconCache.insert(themePath + path, icon);
	}

	return IconCache.value(themePath + path);
}

void IconsManager::clearCache()
{
	IconCache.clear();
}

void IconsManager::configurationUpdated()
{
	bool themeWasChanged = Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "IconTheme") != ThemeManager->currentTheme().name();
	if (themeWasChanged)
	{
		clearCache();
		ThemeManager->setCurrentTheme(Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "IconTheme"));
		Application::instance()->configuration()->deprecatedApi()->writeEntry("Look", "IconTheme", ThemeManager->currentTheme().name());

		emit themeChanged();
	}
}

QSize IconsManager::getIconsSize()
{
	return QSize(16, 16);
}

#include "moc_icons-manager.cpp"
