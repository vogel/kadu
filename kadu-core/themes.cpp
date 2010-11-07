/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFile>

#include "configuration/configuration-file.h"

#include "debug.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"

#include "themes.h"

Themes::Themes(const QString &themename, const QString &configname)
	: QObject(), ThemesList(), ThemesPaths(), additional(),
	ConfigName(configname), Name(themename), ActualTheme("Custom"), entries()
{
	setPaths(QStringList());
}

QStringList Themes::getSubDirs(const QString &path, bool validate) const
{
	QDir dir(path);
	dir.setFilter(QDir::Dirs);
	QStringList dirs = dir.entryList();
	dirs.removeAll(".");
	dirs.removeAll("..");

	if (!validate)
		return dirs;

	QStringList subdirs;
	foreach(const QString &dir, dirs)
	{
		QString dirname = path + '/' + dir;
		if (validateDir(dirname))
			subdirs.append(dir);
	}
	return subdirs;
}

bool Themes::validateDir(const QString &path) const
{
	if (ConfigName.isEmpty())
		return true;

	QFile f(path + '/' + ConfigName);
	if (f.exists())
		return true;

	QStringList subdirs = getSubDirs(path, false);
	if (!subdirs.isEmpty())
	{
		foreach(const QString &dir, subdirs)
		{
			f.setFileName(path + '/' + dir + '/' + ConfigName);
			if (!f.exists())
				return false;
		}

		return true;
	}

	return false;
}

const QStringList & Themes::themes() const
{
	return ThemesList;
}

void Themes::setTheme(const QString &theme)
{
	kdebugf();

	if (ThemesList.contains(theme) || (theme == "Custom"))
	{
		entries.clear();
		ActualTheme = theme;
		if (theme != "Custom" && !ConfigName.isEmpty())
		{
			PlainConfigFile theme_file(
			themePath() +  fixFileName(themePath(), ConfigName));
			theme_file.read();
			entries = theme_file.getGroupSection(Name);
		}
		emit themeChanged(ActualTheme);
	}

	kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_INFO, "end: theme: %s\n", qPrintable(ActualTheme));
}

const QString & Themes::theme() const
{
	return ActualTheme;
}

void Themes::setPaths(const QStringList &paths)
{
	kdebugf();
	ThemesList.clear();
	ThemesPaths.clear();
	additional.clear();
	QStringList temp = paths + defaultKaduPathsWithThemes();
	foreach(const QString &it, temp)
	{
		if (validateDir(it))
		{
			if (paths.indexOf(it) != -1)
				additional.append(it);
			ThemesPaths.append(it);
			ThemesList.append(it.section('/', -1, -1, QString::SectionSkipEmpty));
		}
// TODO: 0.6.5
// 		else
// 			MessageDialog::msg(tr("<i>%1</i><br/>does not contain any theme configuration file").arg(it), false, "dialog-warning");
	}
	emit pathsChanged(ThemesPaths);
	kdebugf2();
}

QStringList Themes::defaultKaduPathsWithThemes() const
{
	QStringList result;

	foreach(const QString &it, getSubDirs(dataPath("kadu/themes/" + Name)))
		result << dataPath("kadu/themes/" + Name + '/' + it + '/');

	foreach(const QString &it, getSubDirs(profilePath(Name)))
		result << profilePath(Name) + '/' + it + '/';

	return result;
}

const QStringList & Themes::paths() const
{
    return ThemesPaths;
}

const QStringList & Themes::additionalPaths() const
{
    return additional;
}

QString Themes::themePath(const QString &theme) const
{
	QString t = theme;
	if (theme.isEmpty())
		t = ActualTheme;
	if (t == "Custom")
		return QString::null;
	if (ThemesPaths.isEmpty())
		return "Custom";

	QRegExp r("(/" + t + "/)$");
	foreach (QString theme, ThemesPaths)
		if (-1 != r.indexIn(theme))
			return theme;

	return "Custom";;
}

QString Themes::getThemeEntry(const QString &name) const
{
	if (entries.contains(name))
		return entries[name];
	else
		return QString::null;
}
