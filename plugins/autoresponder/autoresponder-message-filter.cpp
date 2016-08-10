/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "autoresponder-message-filter.h"

#include "autoresponder-configuration-ui-handler.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "message/message-filter-service.h"
#include "message/message-manager.h"
#include "parser/parser.h"
#include "protocols/protocol.h"
#include "status/status-type-data.h"
#include "status/status-type-group.h"
#include "status/status-type-manager.h"
#include "debug.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>

AutoresponderMessageFilter::AutoresponderMessageFilter(QObject *parent) :
		QObject(parent)
{
}

AutoresponderMessageFilter::~AutoresponderMessageFilter()
{
}

void AutoresponderMessageFilter::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
}

void AutoresponderMessageFilter::setMessageManager(MessageManager *messageManager)
{
	m_messageManager = messageManager;
}

void AutoresponderMessageFilter::setParser(Parser *parser)
{
	m_parser = parser;
}

void AutoresponderMessageFilter::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
	m_statusTypeManager = statusTypeManager;
}

void AutoresponderMessageFilter::init()
{
	connect(m_chatWidgetRepository, SIGNAL(chatWidgetRemoved(ChatWidget *)),
			this, SLOT(chatWidgetClosed(ChatWidget *)));
}

bool AutoresponderMessageFilter::acceptMessage(const Message &message)
{
	if (MessageTypeSent == message.type())
		return true;

	if (htmlToPlain(message.content()).left(5) == "KADU ") // ignore other kadu autoresponses
		return true;

	if (!Configuration.respondConferences() && (message.messageChat().contacts().count() > 1))
		return true;

	if (Configuration.respondOnlyFirst() && RepliedChats.contains(message.messageChat()))
		return true;

	Protocol *protocol = message.messageChat().chatAccount().protocolHandler();
	if (!protocol)
		return true;

	// Na chwilę obecną busy == away
	auto group = m_statusTypeManager->statusTypeData(protocol->status().type()).typeGroup();
	if ((Configuration.statusAvailable() && group == StatusTypeGroup::Online)
			|| (Configuration.statusInvisible() && group == StatusTypeGroup::Invisible)
			|| (Configuration.statusBusy() && group == StatusTypeGroup::Away))
	{
		m_messageManager->sendMessage(message.messageChat(), normalizeHtml(HtmlString{tr("KADU AUTORESPONDER:") + '\n'
				+ m_parser->parse(Configuration.autoRespondText(), Talkable(message.messageSender()), ParserEscape::HtmlEscape)}), true);

		RepliedChats.insert(message.messageChat());
	}

	return true;
}

void AutoresponderMessageFilter::chatWidgetClosed(ChatWidget *chatWidget)
{
	RepliedChats.remove(chatWidget->chat());
}

void AutoresponderMessageFilter::setConfiguration(const AutoresponderConfiguration &configuration)
{
	Configuration = configuration;
}

/** @} */

#include "moc_autoresponder-message-filter.cpp"
