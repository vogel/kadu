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

#include "message-render-info-builder.h"

#include "message/message-render-info.h"

MessageRenderInfoBuilder::MessageRenderInfoBuilder() :
		m_includeHeader{true}, m_separatorSize{}, m_showServerTime{}
{
}

MessageRenderInfoBuilder & MessageRenderInfoBuilder::setMessage(Message message)
{
	m_message = std::move(message);
	return *this;
}

MessageRenderInfoBuilder & MessageRenderInfoBuilder::setBackgroundColor(QString backgroundColor)
{
	m_backgroundColor = std::move(backgroundColor);
	return *this;
}

MessageRenderInfoBuilder & MessageRenderInfoBuilder::setFontColor(QString fontColor)
{
	m_fontColor = std::move(fontColor);
	return *this;
}

MessageRenderInfoBuilder & MessageRenderInfoBuilder::setNickColor(QString nickColor)
{
	m_nickColor = std::move(nickColor);
	return *this;
}

MessageRenderInfoBuilder &MessageRenderInfoBuilder::setIncludeHeader(bool includeHeader)
{
	m_includeHeader = includeHeader;
	return *this;
}

MessageRenderInfoBuilder & MessageRenderInfoBuilder::setSeparatorSize(int separatorSize)
{
	m_separatorSize = separatorSize;
	return *this;
}

MessageRenderInfoBuilder & MessageRenderInfoBuilder::setShowServerTime(bool showServerTime)
{
	m_showServerTime = showServerTime;
	return *this;
}

MessageRenderInfo MessageRenderInfoBuilder::create()
{
	return
	{
		m_message,
		m_backgroundColor,
		m_fontColor,
		m_nickColor,
		m_includeHeader,
		m_separatorSize,
		m_showServerTime
	};
}
