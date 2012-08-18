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

#ifndef MESSAGE_HTML_RENDERER_SERVICE_H
#define MESSAGE_HTML_RENDERER_SERVICE_H

#include <QtCore/QObject>

class Message;

/**
 * @addtogroup Message
 * @{
 */

/**
 * @class MessageHtmlRendererService
 * @short Service for rendering messages into HTML.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service is used to render Message into HTML fragment.
 */
class MessageHtmlRendererService : public QObject
{
	Q_OBJECT

public:
	explicit MessageHtmlRendererService(QObject *parent = 0);
	virtual ~MessageHtmlRendererService();

	/**
	 * @short Render Message into HTML fragment,
	 * @author Rafał 'Vogel' Malinowski
	 * @param message to render
	 * @return HTML representation of Message
	 */
	QString renderMessage(const Message &message);

};

/**
 * @}
 */

#endif // MESSAGE_HTML_RENDERER_SERVICE_H
