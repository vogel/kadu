/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include "misc/kadu-paths.h"

#include "syntax-list.h"

SyntaxList::SyntaxList(const QString &category)
	: category(category)
{
	reload();
}

QString SyntaxList::readSyntax(const QString &category, const QString &name, const QString &defaultSyntax)
{
	QString path;
	QFile syntaxFile;
	path = KaduPaths::instance()->dataPath() + QLatin1String("syntax/") + category + '/' + name + QLatin1String(".syntax");

	syntaxFile.setFileName(path);
	if (!syntaxFile.open(QIODevice::ReadOnly))
	{
		path = KaduPaths::instance()->profilePath() + QLatin1String("syntax/") + category + '/' + name + QLatin1String(".syntax");

		syntaxFile.setFileName(path);
		if (!syntaxFile.open(QIODevice::ReadOnly))
			return defaultSyntax;
	}

	QString result;
	QTextStream stream(&syntaxFile);
	stream.setCodec("UTF-8");
	result = stream.readAll();
	syntaxFile.close();

	if (result.isEmpty())
		return defaultSyntax;

	return result;
}

void SyntaxList::reload()
{
	QDir dir;
	QString path;
	QFileInfo fi;
	QStringList files;

	SyntaxInfo info;

	info.global = false;
	path = KaduPaths::instance()->profilePath() + QLatin1String("syntax/") + category + '/';
	dir.setPath(path);

	dir.setNameFilters(QStringList("*.syntax"));
	files = dir.entryList();

	foreach(const QString &file, files)
	{
		fi.setFile(path + file);
		if (fi.isReadable())
			insert(fi.baseName(), info);
	}

	info.global = true;
	path = KaduPaths::instance()->dataPath() + QLatin1String("syntax/") + category + '/';
	dir.setPath(path);

	files = dir.entryList();

	foreach(const QString &file, files)
	{
		fi.setFile(path + file);
		if (fi.isReadable() && !contains(file))
			insert(fi.baseName(), info);
	}

	if (isEmpty())
	{
		info.global = false;
		insert("custom", info);

		updateSyntax("custom", QString());
	}
}

bool SyntaxList::updateSyntax(const QString &name, const QString &syntax)
{
	QString path = KaduPaths::instance()->profilePath() + QLatin1String("syntax/");
	QDir dir(path);
	if (!dir.exists())
		if (!dir.mkdir(path))
			return false;

	path = KaduPaths::instance()->profilePath() + QLatin1String("syntax/") + category + '/';
	dir.setPath(path);
	if (!dir.exists())
		if (!dir.mkdir(path))
			return false;

	QFile syntaxFile;
	syntaxFile.setFileName(path + name + ".syntax");
	if (!syntaxFile.open(QIODevice::WriteOnly))
		return false;

	QTextStream stream(&syntaxFile);
	stream.setCodec("UTF-8");
	stream << syntax;
	syntaxFile.close();

	SyntaxInfo info;
	info.global = false;
	insert(name, info);

	emit updated();

	return true;
}

QString SyntaxList::readSyntax(const QString &name)
{
	if (!contains(name))
		return QString();

	SyntaxInfo info = *(find(name));
	QString path;
	if (info.global)
		path = KaduPaths::instance()->dataPath() + QLatin1String("syntax/") + category + '/' + name + QLatin1String(".syntax");
	else
		path = KaduPaths::instance()->profilePath() + QLatin1String("syntax/") + category + '/' + name + QLatin1String(".syntax");

	QFile syntaxFile;
	syntaxFile.setFileName(path);
	if (!syntaxFile.open(QIODevice::ReadOnly))
		return QString();

	QString result;
	QTextStream stream(&syntaxFile);
	stream.setCodec("UTF-8");
	result = stream.readAll();
	syntaxFile.close();

	return result;
}

bool SyntaxList::deleteSyntax(const QString &name)
{
	if (!contains(name))
		return false;

	SyntaxInfo info = *(find(name));
	if (info.global)
		return false;

	QString path = KaduPaths::instance()->profilePath() + QLatin1String("syntax/") + category + '/' + name + QLatin1String(".syntax");
	QFile file;
	file.setFileName(path);

	if (!file.remove())
		return false;

	remove(name);
	emit updated();

	return true;
}

bool SyntaxList::isGlobal(const QString &name)
{
	if (!contains(name))
		return false;

	SyntaxInfo info = *(find(name));
	return info.global;
}

#include "moc_syntax-list.cpp"
