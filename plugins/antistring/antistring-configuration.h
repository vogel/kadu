/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ANTISTRING_CONFIGURATION_H
#define ANTISTRING_CONFIGURATION_H

#include <QtCore/QList>
#include <QtCore/QPair>

#include "configuration/configuration-aware-object.h"

typedef QPair<QString, int> ConditionPair;

class AntistringConfiguration : public ConfigurationAwareObject
{
	QList<ConditionPair> Conditions;

	bool Enabled;
	bool MessageStop;
	bool LogMessage;
	QString ReturnMessage;
	QString LogFile;

	void createDefaultConfiguration();
	void addCondition(const QString &conditionString);
	void readDefaultConditions();

protected:
	virtual void configurationUpdated();

public:
	AntistringConfiguration();
	virtual ~AntistringConfiguration();

	void readConditions();
	void storeConditions();

	QList<QPair<QString, int> > & conditions() { return Conditions; }

	bool enabled() const { return Enabled; }
	bool messageStop() const { return MessageStop; }
	bool logMessage() const { return LogMessage; }
	const QString & returnMessage() const { return ReturnMessage; }
	const QString & logFile() const { return LogFile; }

};

#endif // ANTISTRING_CONFIGURATION_H
