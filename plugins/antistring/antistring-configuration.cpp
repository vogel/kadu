/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "debug.h"

#include "antistring-configuration.h"

AntistringConfiguration::AntistringConfiguration()
{
	createDefaultConfiguration();
	configurationUpdated();
}

AntistringConfiguration::~AntistringConfiguration()
{
}

void AntistringConfiguration::createDefaultConfiguration()
{
	config_file.addVariable("PowerKadu", "log file", KaduPaths::instance()->profilePath() + QLatin1String("antistring.log"));
	config_file.addVariable("PowerKadu", "admonish_tresc_config",
			"http://www.olsztyn.mm.pl/~silentman/lancuszki.htm");
}

void AntistringConfiguration::configurationUpdated()
{
	readConditions();

	Enabled = config_file.readBoolEntry("PowerKadu", "enable_antistring");
	MessageStop = config_file.readBoolEntry("PowerKadu", "message stop");
	LogMessage = config_file.readBoolEntry("PowerKadu", "log message");
	ReturnMessage = config_file.readEntry("PowerKadu", "admonish_tresc_config");
	LogFile = config_file.readEntry("PowerKadu", "log file", KaduPaths::instance()->profilePath() + QLatin1String("antistring.log"));
}

void AntistringConfiguration::addCondition(const QString &conditionString)
{
	QStringList conditionPair = conditionString.split('\t');
	if (conditionPair.isEmpty())
		return;

	bool ok;
	int factor = conditionPair[0].toInt(&ok, 10);
	if (ok)
		// TODO why we are not checking if there are two items?
		Conditions.append(qMakePair(conditionPair[1], factor));
}

void AntistringConfiguration::readDefaultConditions()
{
	QFile defaultListFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/data/antistring/ant_conditions.conf"));
	if (!defaultListFile.open(QFile::ReadOnly))
	{
		kdebug("Can't open file: %s\n", qPrintable(defaultListFile.fileName()));
		return;
	}

	QTextStream textStream(&defaultListFile);

	while (!textStream.atEnd())
		addCondition(textStream.readLine());

	defaultListFile.close();
}

void AntistringConfiguration::readConditions()
{
	Conditions.clear();

	QString conditionsString = config_file.readEntry("PowerKadu", "antistring conditions");
	QStringList conditionsList = conditionsString.split("\t\t");

	if (conditionsList.empty())
	{
		readDefaultConditions();
		return;
	}

	foreach (const QString &conditionItem, conditionsList)
		addCondition(conditionItem);
}

void AntistringConfiguration::storeConditions()
{
	QStringList conditionsList;

	foreach (const ConditionPair &condition, Conditions)
		conditionsList.append(QString::number(condition.second) + '\t' + condition.first);

	config_file.writeEntry("PowerKadu", "antistring conditions", conditionsList.join("\t\t"));
}
