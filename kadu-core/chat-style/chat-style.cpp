/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-style.h"

ChatStyle::ChatStyle()
{
}

ChatStyle::ChatStyle(QString name, QString variant) :
		m_name{std::move(name)},
		m_variant{std::move(variant)}
{
}

QString ChatStyle::name() const
{
	return m_name;
}

QString ChatStyle::variant() const
{
	return m_variant;
}

bool operator == (const ChatStyle &left, const ChatStyle &right)
{
	if (left.name() != right.name())
		return false;
	if (left.variant() != right.variant())
		return false;
	return true;
}

bool operator != (const ChatStyle &left, const ChatStyle &right)
{
	return !(left == right);
}

bool operator < (const ChatStyle &left, const ChatStyle &right)
{
	if (left.name() < right.name())
		return true;
	if (left.name() > right.name())
		return false;
	return left.variant() < right.variant();
}

bool operator <= (const ChatStyle &left, const ChatStyle &right)
{
	return !(right < left);
}

bool operator > (const ChatStyle &left, const ChatStyle &right)
{
	return right < left;
}

bool operator >= (const ChatStyle &left, const ChatStyle &right)
{
	return !(left < right);
}
