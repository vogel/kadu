/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Michał Obrembski (byku@byku.com.pl)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QString>

#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "message/message-manager.h"
#include "message/message-render-info.h"
#include "protocols/services/chat-service.h"
#include "debug.h"

#include "image-link.h"

ImageLink * ImageLink::Instance = 0;

void ImageLink::createInstance()
{
	if (!Instance)
		Instance = new ImageLink();
}

void ImageLink::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

ImageLink::ImageLink()
{
	ImageRegExp = QRegExp("http://.*(.gif|.*.jpg|.*.png)");
	YouTubeRegExp = QRegExp("http://www.youtube.com/watch(.*)&?");

	triggerAllAccountsRegistered();
}

ImageLink::~ImageLink()
{
}

void ImageLink::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();

	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();

	if (chatService)
		connect(chatService, SIGNAL(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)),
		        this, SLOT(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)));
}


void ImageLink::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();

	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();

	if (chatService)
		disconnect(chatService, SIGNAL(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)),
		           this, SLOT(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)));
}

void ImageLink::filterIncomingMessage(Chat chat, Contact sender, QString &message, time_t time, bool &ignore)
{
	Q_UNUSED(time)
	Q_UNUSED(ignore)
	Q_UNUSED(sender)

	if (Configuration.showImages())
	{
		ImageRegExp.indexIn(message);
		QStringList list = ImageRegExp.capturedTexts();

		if (ImageRegExp.matchedLength() > 0 && !list.isEmpty())
			insertCodeIntoChatWindow(chat, sender, getImageCode(list[0]));
	}

	if (Configuration.showVideos())
	{
		YouTubeRegExp.indexIn(message);
		QStringList list = YouTubeRegExp.capturedTexts();

		if (YouTubeRegExp.matchedLength() > 0 && list.size() > 1)
			insertCodeIntoChatWindow(chat, sender, getVideoCode(list[1]));
	}

	kdebugf2();
}

QString ImageLink::getImageCode(const QString &image)
{
	return QString("<img src=\"%1\">").arg(image);
}

QString ImageLink::getVideoCode(const QString &video)
{
	QString url(video);
	url.remove("?v=");
	return QString(
		"<object width=\"33%\" height=\"33%\">"
			"<embed "
				"src=\"http://www.youtube.com/v/%1&autoplay=%2 \" "
				"type=\"application/x-shockwave-flash\" "
				"allowscriptaccess=\"always\" "
				"allowfullscreen=\"true\" "
				"width=\"33%\" "
				"height=\"33%\">"
			"</embed>"
		"</object>").arg(url).arg(Configuration.autoStartVideos() ? "1" : "0");
}

void ImageLink::insertCodeIntoChatWindow(Chat chat, Contact sender, const QString &code)
{
	Message message = Message::create();
	message.setMessageChat(chat);
	message.setType(MessageTypeReceived);
	message.setMessageSender(sender);
	message.setContent(code);
	message.setReceiveDate(QDateTime::currentDateTime());
	message.setSendDate(QDateTime::currentDateTime());

	MessageManager::instance()->addUnreadMessage(message);

	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat, false);
	if (!chatWidget)
		ChatWidgetManager::instance()->messageReceived(message);
	else
		chatWidget->chatMessagesView()->appendMessage(message);
}
