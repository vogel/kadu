/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtXmlPatterns/QXmlQuery>

#include "gadu-avatar-data-parser.h"

#define DEFAULT_DELAY 7200

const QString GaduAvatarDataParser::AvatarQuery("/result/users/user[@uin='%1']/avatars/avatar[@order='0' and @blank='']/originBigAvatar/text()");
const QString GaduAvatarDataParser::BlankAvatarQuery("/result/users/user[@uin='%1']/avatars/avatar[@order='0']/@blank/text()");
const QString GaduAvatarDataParser::TimestampQuery("/result/users/user[@uin='%1']/avatars/avatar[@order='0' and @blank='']/timestamp/text()");
const QString GaduAvatarDataParser::PackageDelayQuery("/result/users/user[@uin='%1']/avatars/avatar[@order='0' and @blank='']/packageDelay/text()");

GaduAvatarDataParser::GaduAvatarDataParser(QIODevice *ioDevice, QString contactId) :
		IsValid(false), IsBlank(false), Delay(DEFAULT_DELAY)
{
	if (ioDevice)
	{
		if (ioDevice->open(QIODevice::ReadOnly | QIODevice::Text))
		{
			parseData(ioDevice, contactId);
			ioDevice->close();
		}
	}
}

void GaduAvatarDataParser::parseData(QIODevice *ioDevice, QString contactId)
{
	QXmlQuery xmlQuery;
	xmlQuery.setFocus(ioDevice);
	xmlQuery.setQuery(AvatarQuery.arg(contactId));

	if (xmlQuery.evaluateTo(&AvatarUrl))
	{
		AvatarUrl = AvatarUrl.trimmed();
		if (!AvatarUrl.isEmpty())
			IsValid = true;
		IsBlank = false;
	}

	if (AvatarUrl.isEmpty())
	{
		QString blankString;
		xmlQuery.setQuery(BlankAvatarQuery.arg(contactId));
		if (xmlQuery.evaluateTo(&blankString))
		{
			if (blankString == "1")
			{
				IsValid = true;
				IsBlank = true;
			}
			else
				return; // invalid data
		}
		else
			return; // invalid data
	}

	QString timestampString;
	xmlQuery.setQuery(TimestampQuery.arg(contactId));
	if (xmlQuery.evaluateTo(&timestampString))
		Timestamp = QDateTime::fromString(timestampString, Qt::ISODate);
	else
		Timestamp = QDateTime::currentDateTime();

	QString delayString;
	xmlQuery.setQuery(TimestampQuery.arg(contactId));
	if (xmlQuery.evaluateTo(&delayString))
		Delay = delayString.toInt();
	if (0 == Delay)
		Delay = DEFAULT_DELAY;
}
