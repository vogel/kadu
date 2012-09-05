/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtCore/QVector>

#include "themes/theme.h"

#include "theme/emoticon-theme.h"
#include "theme/gadu-emoticon-parser.h"

#include "gadu-emoticon-theme-loader.h"

bool GaduEmoticonThemeLoader::validGaduGaduEmoticonDirectory(const QString &dir)
{
	return QFileInfo(dir + "/emots.txt").exists();
}

void GaduEmoticonThemeLoader::loadEmoticons(const QString &path)
{
	if (path.isEmpty())
		return;

	QString dir = path.endsWith('/') ? path : path + '/';

	if (!validGaduGaduEmoticonDirectory(dir))
		return;

	QFile emotsTxtFile(dir + "emots.txt");
	if (!emotsTxtFile.open(QIODevice::ReadOnly))
		return;

	QTextStream emotsTxtStream(&emotsTxtFile);
	emotsTxtStream.setCodec(QTextCodec::codecForName("CP1250"));
	while (!emotsTxtStream.atEnd())
	{
		GaduEmoticonParser parser(dir, emotsTxtStream.readLine());
		if (!parser.emoticon().isNull())
			Emoticons.append(parser.emoticon());
		foreach (const Emoticon &alias, parser.aliases())
			if (!alias.isNull())
				Aliases.append(alias);
	}

	emotsTxtFile.close();
}

EmoticonTheme GaduEmoticonThemeLoader::loadEmoticonTheme(const QString &path)
{
	loadEmoticons(path);

	QDir themeDir(path);
	QFileInfoList subDirs = themeDir.entryInfoList(QDir::Dirs);

	foreach (const QFileInfo &subDirInfo, subDirs)
	{
		if (subDirInfo.fileName().startsWith('.'))
			continue;

		QString subDir = subDirInfo.canonicalFilePath();
		if (validGaduGaduEmoticonDirectory(subDir))
			loadEmoticons(subDir);
	}

	return EmoticonTheme(Emoticons, Aliases);
}
