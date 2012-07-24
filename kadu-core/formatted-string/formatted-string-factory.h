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

#ifndef FORMATTED_STRING_FACTORY_H
#define FORMATTED_STRING_FACTORY_H

class QString;

class FormattedMessage;

/**
 * @class FormattedStringFactory
 * @short Class for creating FormattedMessage instances from different sources.
 * @author Rafał 'Vogel' Malinowski
 */
class FormattedStringFactory
{
public:
	/**
	 * @short Create FormattedMessage instance with plain text.
	 * @author Rafał 'Vogel' Malinowski
	 * @param plainText plain content of new FormattedMessage
	 *
	 * This method will create empty FormattedMessage if plainText is empty. If plainText is not empty then returned FormattedMessage
	 * will contain one text part.
	 */
	FormattedMessage fromPlainText(const QString &plainText);

};

#endif // FORMATTED_STRING_FACTORY_H
