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

#include "raw-message.h"

RawMessage::RawMessage()
{
}

RawMessage::RawMessage(QByteArray rawPlainContent, QByteArray rawXmlContent) :
		m_rawPlainContent{std::move(rawPlainContent)},
		m_rawXmlContent{std::move(rawXmlContent)}
{
}

RawMessage::RawMessage(const QByteArray &plainContent) :
		m_rawPlainContent{plainContent},
		m_rawXmlContent{plainContent}
{
}

QByteArray RawMessage::rawContent() const
{
	return m_rawXmlContent.isEmpty()
		? m_rawPlainContent
		: m_rawXmlContent;
}

QByteArray RawMessage::rawPlainContent() const
{
	return m_rawPlainContent;
}

QByteArray RawMessage::rawXmlContent() const
{
	return m_rawXmlContent;
}
