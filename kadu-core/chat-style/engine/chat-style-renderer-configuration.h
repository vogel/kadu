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

#include "chat/chat.h"

class QWebFrame;

/**
 * @addtogroup ChatStyle
 * @{
 */

/**
 * @class ChatStyleRendererConfiguration
 * @short Configuration for ChatStyleRenderer.
 *
 * This class contains all data required to properlt set up ChatStyleRenderer. It is: chat, web frame used
 * to display chat messages, default javascript code and transparency setting.
 */
class ChatStyleRendererConfiguration
{

public:
	ChatStyleRendererConfiguration(Chat chat, QWebFrame &webFrame, QString javaScript, bool useTransparency);

	Chat chat() const;
	QWebFrame & webFrame() const;
	QString javaScript() const;
	bool useTransparency() const;

private:
	Chat m_chat;
	QWebFrame &m_webFrame;
	QString m_javaScript;
	bool m_useTransparency;

};

bool operator == (const ChatStyleRendererConfiguration &left, const ChatStyleRendererConfiguration &right);
bool operator != (const ChatStyleRendererConfiguration &left, const ChatStyleRendererConfiguration &right);

/**
 * @}
 */
