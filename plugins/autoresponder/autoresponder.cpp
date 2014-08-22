/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Dariusz Markowicz (darom@alari.pl)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtGui/QApplication>
#include <QtGui/QLineEdit>

#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "message/message-manager.h"
#include "parser/parser.h"
#include "protocols/protocol.h"
#include "services/message-filter-service.h"
#include "status/status-type-group.h"
#include "debug.h"

#include "autoresponder-configuration-ui-handler.h"
#include "autoresponder-configurator.h"

#include "autoresponder.h"

AutoResponder::AutoResponder(QObject *parent) :
		MessageFilter(parent)
{
}

AutoResponder::~AutoResponder()
{
}

bool AutoResponder::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	connect(Core::instance()->chatWidgetRepository(), SIGNAL(chatWidgetRemoved(ChatWidget *)),
			this, SLOT(chatWidgetClosed(ChatWidget *)));

	UiHandler = new AutoresponderConfigurationUiHolder(this);
	MainConfigurationWindow::registerUiHandler(UiHandler);

	Configurator = new AutoresponderConfigurator();
	Configurator->setAutoresponder(this);

	Core::instance()->messageFilterService()->registerMessageFilter(this);

	return true;
}

void AutoResponder::done()
{
	Core::instance()->messageFilterService()->unregisterMessageFilter(this);

	MainConfigurationWindow::unregisterUiHandler(UiHandler);

	delete Configurator;
	Configurator = 0;

	disconnect(Core::instance()->chatWidgetRepository(), 0, this, 0);
}

bool AutoResponder::acceptMessage(const Message &message)
{
	if (MessageTypeSent == message.type())
		return true;

	if (message.plainTextContent().left(5) == "KADU ") // ignore other kadu autoresponses
		return true;

	if (!Configuration.respondConferences() && (message.messageChat().contacts().count() > 1))
		return true;

	if (Configuration.respondOnlyFirst() && RepliedChats.contains(message.messageChat()))
		return true;

	Protocol *protocol = message.messageChat().chatAccount().protocolHandler();
	if (!protocol)
		return true;

	// Na chwilę obecną busy == away
	if ((Configuration.statusAvailable() && protocol->status().group() == StatusTypeGroupOnline)
			|| (Configuration.statusInvisible() && protocol->status().group() == StatusTypeGroupInvisible)
			|| (Configuration.statusBusy() && protocol->status().group() == StatusTypeGroupAway))
	{
		MessageManager::instance()->sendMessage(message.messageChat(), tr("KADU AUTORESPONDER:") + '\n'
				+ Parser::parse(Configuration.autoRespondText(), Talkable(message.messageSender()), ParserEscape::HtmlEscape), true);

		RepliedChats.insert(message.messageChat());
	}

	return true;
}

void AutoResponder::chatWidgetClosed(ChatWidget *chatWidget)
{
	RepliedChats.remove(chatWidget->chat());
}

void AutoResponder::setConfiguration(const AutoresponderConfiguration &configuration)
{
	Configuration = configuration;
}

Q_EXPORT_PLUGIN2(autoresponder, AutoResponder)

/** @} */

#include "moc_autoresponder.cpp"
