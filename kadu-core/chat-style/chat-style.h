/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QString>

/**
 * @addtogroup ChatStyle
 * @{
 */

/**
 * @class ChatStyle
 * @short Chat style definition.
 *
 * This class contains only two fields - name of style and its variant.
 */
class ChatStyle
{

public:
	/**
	 * @short Create new empty ChatStyle.
	 */
	ChatStyle();

	/**
	 * @short Create ChatStyle with given name and variant.
	 */
	ChatStyle(QString name, QString variant);

	/**
	 * @return name of style
	 */
	QString name() const;

	/**
	 * @return variant of style
	 */
	QString variant() const;

private:
	QString m_name;
	QString m_variant;

};

bool operator == (const ChatStyle &left, const ChatStyle &right);
bool operator != (const ChatStyle &left, const ChatStyle &right);
bool operator < (const ChatStyle &left, const ChatStyle &right);
bool operator <= (const ChatStyle &left, const ChatStyle &right);
bool operator > (const ChatStyle &left, const ChatStyle &right);
bool operator >= (const ChatStyle &left, const ChatStyle &right);

/**
 * @}
 */
