/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtCore/QTextStream>

#include "configuration/configuration-file.h"
#include "misc/kadu-paths.h"

#include "cenzor-configuration.h"

QStringList CenzorConfiguration::toStringList(const QList<QRegExp> &list)
{
	QStringList result;
	foreach (const QRegExp &regExp, list)
		result.append(regExp.pattern());
	return result;
}

QList<QRegExp> CenzorConfiguration::toRegExpList(const QStringList &list)
{
	QList<QRegExp> result;
	foreach (const QString &item, list)
		result.append(QRegExp(item));
	return result;
}

CenzorConfiguration::CenzorConfiguration()
{
	configurationUpdated();
}

CenzorConfiguration::~CenzorConfiguration()
{
}

void CenzorConfiguration::setExclusionList(const QList<QRegExp> &exclusionList)
{
	ExclusionList = exclusionList;
}

void CenzorConfiguration::setSwearList(const QList<QRegExp> &swearList)
{
	SwearList = swearList;
}

QList<QRegExp> CenzorConfiguration::loadRegExpList(const QString &itemName, const QString &fileName)
{
	QList<QRegExp> result = toRegExpList(config_file.readEntry("PowerKadu", itemName).split('\t', QString::SkipEmptyParts));

	if (!result.empty())
		return result;

	QFile file(fileName);

	if (!file.open(QFile::ReadOnly))
		return result;

	QTextStream stream(&file);
	while (!stream.atEnd())
		result += QRegExp(stream.readLine());
	file.close();

	return result;
}

void CenzorConfiguration::configurationUpdated()
{
	Enabled = config_file.readBoolEntry("PowerKadu", "enable_cenzor");
	Admonition = config_file.readEntry("PowerKadu", "admonition_content_cenzor", "Cenzor: Watch your mouth!! <nonono>");
	SwearList = loadRegExpList("cenzor swearwords", KaduPaths::instance()->dataPath() + QLatin1String("plugins/data/cenzor/cenzor_words.conf"));
	ExclusionList = loadRegExpList("cenzor exclusions", KaduPaths::instance()->dataPath() + QLatin1String("plugins/data/cenzor/cenzor_words_ok.conf"));
}

void CenzorConfiguration::saveConfiguration()
{
	config_file.writeEntry("PowerKadu", "cenzor swearwords", toStringList(SwearList).join("\t"));
	config_file.writeEntry("PowerKadu", "cenzor exclusions", toStringList(ExclusionList).join("\t"));
}
