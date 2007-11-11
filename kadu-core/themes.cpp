/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_file.h"
#include "debug.h"
#include "message_box.h"
#include "misc.h"
#include "themes.h"

Themes::Themes(const QString& themename, const QString& configname, const char *name)
	: QObject(NULL, name), ThemesList(), ThemesPaths(), additional(),
	ConfigName(configname), Name(themename), ActualTheme("Custom"), entries()
{
	setPaths(QStringList());
}

QStringList Themes::getSubDirs(const QString& path, bool validate) const
{
	QDir dir(path);
	dir.setFilter(QDir::Dirs);
	QStringList dirs = dir.entryList();
	dirs.remove(".");
	dirs.remove("..");

	if (!validate)
		return dirs;

	QStringList subdirs;
	CONST_FOREACH(dir, dirs)
	{
		QString dirname = path + '/' + (*dir);
		if (validateDir(dirname))
			subdirs.append(*dir);
	}
	return subdirs;
}

bool Themes::validateDir(const QString& path) const
{
	QFile f(path + '/' + ConfigName);
	if (f.exists())
		return true;

	QStringList subdirs = getSubDirs(path, false);
	if (!subdirs.isEmpty())
	{
		CONST_FOREACH(dir, subdirs)
		{
			f.setName(path + '/' + (*dir) + '/' + ConfigName);
			if (!f.exists())
				return false;
		}

		return true;
	}

	return false;
}

const QStringList &Themes::themes() const
{
	return ThemesList;
}

void Themes::setTheme(const QString& theme)
{
	kdebugf();
	if(ThemesList.contains(theme) || (theme == "Custom"))
	{
		entries.clear();
		ActualTheme = theme;
		if (theme != "Custom")
		{
			PlainConfigFile theme_file(
				themePath() +  fixFileName(themePath(), ConfigName));
			entries = theme_file.getGroupSection(Name);
		}
		emit themeChanged(ActualTheme);
	}
	kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_INFO, "end: theme: %s\n", ActualTheme.local8Bit().data());
}

const QString &Themes::theme() const
{
	return ActualTheme;
}

QString Themes::fixFileName(const QString& path, const QString& fn) const
{
	// check if original path is ok
	if(QFile::exists(path + '/' + fn))
		return fn;
	// maybe all lowercase?
	if(QFile::exists(path + '/' + fn.lower()))
		return fn.lower();
	// split for name and extension
	QString name = fn.section('.', 0, 0);
	QString ext = fn.section('.', 1);
	// maybe extension uppercase?
	if(QFile::exists(path + '/' + name + '.' + ext.upper()))
		return name + '.' + ext.upper();
	// we cannot fix it, return original
	return fn;
}

void Themes::setPaths(const QStringList& paths)
{
	kdebugf();
	ThemesList.clear();
	ThemesPaths.clear();
	additional.clear();
	QStringList temp = paths + defaultKaduPathsWithThemes();
	CONST_FOREACH(it, temp)
	{
		if (validateDir(*it))
		{
			if (paths.findIndex(*it) != -1)
				additional.append(*it);
			ThemesPaths.append(*it);
			ThemesList.append((*it).section("/", -2));
		}
		else
			MessageBox::msg(tr("<i>%1</i><br/>does not contain any theme configuration file").arg(*it), false, "Warning");
	}
	emit pathsChanged(ThemesPaths);
	kdebugf2();
}

QStringList Themes::defaultKaduPathsWithThemes() const
{
	QStringList default1, default2;
	default1 = getSubDirs(dataPath("kadu/themes/" + Name));
	default2 = getSubDirs(ggPath(Name));

	FOREACH(it, default1)
		*it = dataPath("kadu/themes/" + Name + '/' + (*it) + '/');

	FOREACH(it, default2)
		*it = ggPath(Name) + '/' + (*it) + '/';

	return default1 + default2;
}

const QStringList &Themes::paths() const
{
    return ThemesPaths;
}

const QStringList &Themes::additionalPaths() const
{
    return additional;
}

QString Themes::themePath(const QString& theme) const
{
	QString t = theme;
	if (theme.isEmpty())
		t = ActualTheme;
	if (theme == "Custom")
		return QString::null;
	if (ThemesPaths.isEmpty())
		return "Custom";
	return ThemesPaths.grep(t).first();
}

QString Themes::getThemeEntry(const QString& name) const
{
	if (entries.contains(name))
		return entries[name];
	else
		return QString::null;
}
