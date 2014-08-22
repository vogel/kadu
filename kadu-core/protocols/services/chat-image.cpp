/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "chat-image.h"

#include <QtCore/QString>

ChatImage::ChatImage() :
		m_size{}
{
}

ChatImage::ChatImage(QString key, quint32 size) :
		m_key{std::move(key)}, m_size{size}
{
}

bool ChatImage::operator == (const ChatImage &compareTo) const
{
	if (m_size != compareTo.m_size)
		return false;
	if (m_key != compareTo.m_key)
		return false;
	return true;
}

bool ChatImage::operator < (const ChatImage &compareTo) const
{
	if (m_size < compareTo.m_size)
		return true;
	if (m_size > compareTo.m_size)
		return false;
	return m_key < compareTo.m_key;
}

bool ChatImage::isNull() const
{
	return m_key.isEmpty() || m_size == 0;
}

QString ChatImage::key() const
{
	return m_key;
}

quint32 ChatImage::size() const
{
	return m_size;
}
