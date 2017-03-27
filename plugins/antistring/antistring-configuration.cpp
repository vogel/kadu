/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "antistring-configuration.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/core.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "misc/paths-provider.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>

AntistringConfiguration::AntistringConfiguration(QObject *parent) :
		QObject{parent}
{
}

AntistringConfiguration::~AntistringConfiguration()
{
}

void AntistringConfiguration::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void AntistringConfiguration::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void AntistringConfiguration::init()
{
	createDefaultConfiguration();
	configurationUpdated();
}

void AntistringConfiguration::createDefaultConfiguration()
{
	m_configuration->deprecatedApi()->addVariable("PowerKadu", "log file", m_pathsProvider->profilePath() + QStringLiteral("antistring.log"));
	m_configuration->deprecatedApi()->addVariable("PowerKadu", "admonish_tresc_config",
			"http://www.olsztyn.mm.pl/~silentman/lancuszki.htm");
}

void AntistringConfiguration::configurationUpdated()
{
	readConditions();

	Enabled = m_configuration->deprecatedApi()->readBoolEntry("PowerKadu", "enable_antistring");
	MessageStop = m_configuration->deprecatedApi()->readBoolEntry("PowerKadu", "message stop");
	LogMessage = m_configuration->deprecatedApi()->readBoolEntry("PowerKadu", "log message");
	ReturnMessage = normalizeHtml(HtmlString{m_configuration->deprecatedApi()->readEntry("PowerKadu", "admonish_tresc_config")});
	LogFile = m_configuration->deprecatedApi()->readEntry("PowerKadu", "log file", m_pathsProvider->profilePath() + QStringLiteral("antistring.log"));
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
	QFile defaultListFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/data/antistring/ant_conditions.conf"));
	if (!defaultListFile.open(QFile::ReadOnly))
		return;

	QTextStream textStream(&defaultListFile);

	while (!textStream.atEnd())
		addCondition(textStream.readLine());

	defaultListFile.close();
}

void AntistringConfiguration::readConditions()
{
	Conditions.clear();

	QString conditionsString = m_configuration->deprecatedApi()->readEntry("PowerKadu", "antistring conditions");
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

	m_configuration->deprecatedApi()->writeEntry("PowerKadu", "antistring conditions", conditionsList.join("\t\t"));
}

#include "moc_antistring-configuration.cpp"
