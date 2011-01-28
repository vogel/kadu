/*
 * %kadu copyright begin%
 * Copyright 2010 Michał Obrembski (byku@byku.com.pl)
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

#include "chat/message/message-render-info.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "debug.h"
#include "gui/widgets/chat-widget-manager.h"

#include "imagelink.h"

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

	QRegExp yt;
	QStringList list;
	kdebugf();
	
	if (Configuration.showImages())
	{
		yt = QRegExp("(http://.*.gif|.*.jpg|.*.png)");
		yt.indexIn(message);
		list = yt.capturedTexts();
		
		if (yt.matchedLength() > 0)
		{
			{
				ChatWidgetManager::instance()->openPendingMessages(chat, false);
				ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat);
				showObject(list[0], 0, chatWidget);
			}
		}
	}
	
	if (Configuration.showVideos())
	{
		yt = QRegExp("http://www.youtube.com/watch(.*)&");
		yt.indexIn(message);
		list = yt.capturedTexts();
		
		if (yt.matchedLength() > 0)
		{
			ChatWidgetManager::instance()->openPendingMessages(chat, false);
			ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat);
			showObject(list[1], 1, chatWidget);
		}
		
		else
		{
			yt = QRegExp("http://www.youtube.com/watch(.*)");
			yt.indexIn(message);
			list = yt.capturedTexts();
			
			if (yt.matchedLength() > 0)
			{
				ChatWidgetManager::instance()->openPendingMessages(chat, false);
				ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat);
				showObject(list[1], 1, chatWidget);
			}
		}
	}
	
	kdebugf2();
}

void ImageLink::showObject(const QString &video, int mode, ChatWidget *widget)
{
	if (!widget) return;
	
	int	width = (widget->width()) / 3;
	int height = (widget->height()) / 3;
	QString messageStr, tmp, tmp2, autoplaystr;
	
	if (Configuration.autoStartVideos())
		autoplaystr.setNum(1);
	else
		autoplaystr.setNum(0);

	if (mode == 1)
	{
		QString url = video;
		url = url.remove("?v=");
		messageStr = QString("<object width=\"%2\" height=\"%3\"><embed src=\"http://www.youtube.com/v/%1&autoplay=%4 \" type=\"application/x-shockwave-flash\" allowscriptaccess=\"always\" allowfullscreen=\"true\" width=\"%2\" height=\"%3\"></embed></object>").arg(url).arg(tmp.setNum(width)).arg(tmp2.setNum(height)).arg(autoplaystr);
	}
	else
		messageStr = QString("<img src=\"%1\">").arg(video);

	Message render = Message::create();

	Chat chat = widget->chat();
	if (chat)
	{
		render.setMessageChat(chat);
		render.setType(Message::TypeSystem);
		render.setMessageSender(chat.contacts().toContact());
		render.setContent(messageStr);
		render.setReceiveDate(QDateTime::currentDateTime());
		render.setSendDate(QDateTime::currentDateTime());
		widget->chatMessagesView()->appendMessage(render);
	}
}
