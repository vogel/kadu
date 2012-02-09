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
 * Copyright 2007, 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"
#include "parser/parser.h"
#include "protocols/services/chat-service.h"
#include "status/status-type-group.h"
#include "debug.h"

#include "autoresponder.h"

/**
 * @ingroup autoresponder
 * @{
 */
AutoResponder::AutoResponder(QObject *parent) :
		ConfigurationUiHandler(parent)
{
	kdebugf();

	triggerAllAccountsRegistered();

	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget *)),
			this, SLOT(chatWidgetClosed(ChatWidget *)));

	createDefaultConfiguration();
	configurationUpdated();

	kdebugf2();
}

AutoResponder::~AutoResponder()
{
	kdebugf();

	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget *)),
			this, SLOT(chatWidgetClosed(ChatWidget *)));

	kdebugf2();
}

int AutoResponder::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	MainConfigurationWindow::registerUiFile(dataPath("plugins/configuration/autoresponder.ui"));
	MainConfigurationWindow::registerUiHandler(this);

	return 0;
}

void AutoResponder::done()
{
	MainConfigurationWindow::unregisterUiHandler(this);
	MainConfigurationWindow::unregisterUiFile(dataPath("plugins/configuration/autoresponder.ui"));
}

void AutoResponder::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (chatService)
	{
		connect(chatService, SIGNAL(filterIncomingMessage(Chat, Contact, QString &, bool &)),
				this, SLOT(filterIncomingMessage(Chat, Contact, QString &, bool &)));
	}
}

void AutoResponder::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (chatService)
	{
		disconnect(chatService, SIGNAL(filterIncomingMessage(Chat, Contact, QString &, bool &)),
				this, SLOT(filterIncomingMessage(Chat, Contact, QString &, bool &)));
	}
}

void AutoResponder::filterIncomingMessage(Chat chat, Contact sender, QString &message, bool &ignore)
{
	Q_UNUSED(ignore)

	kdebugf();
	//kdebugm(KDEBUG_INFO, "Autoresponder received: [%s]\n", qPrintable(message));
	if (message.left(5) == "KADU ")
	{
		kdebugf2();
		return;
	}

	if (!respondConferences && (chat.contacts().count() > 1))
	{
		kdebugf2();
		return;
	}

	if (respondOnlyFirst && repliedUsers.contains(sender))
	{
		kdebugf2();
		return;
	}

	Protocol *protocol = chat.chatAccount().protocolHandler();
	if (!protocol)
	{
		kdebugf2();
		return;
	}

	// Na chwilę obecną busy == away
	if ((statusAvailable && protocol->status().group() == StatusTypeGroupOnline)
			|| (statusInvisible && protocol->status().group() == StatusTypeGroupInvisible)
			|| (statusBusy && protocol->status().group() == StatusTypeGroupAway))
	{
		ChatService *chatService = protocol->chatService();
		if (!chatService)
		{
			kdebugf2();
			return;
		}

		chatService->sendMessage(chat, tr("KADU AUTORESPONDER:") + '\n'
				+ Parser::parse(autoRespondText, Talkable(sender)), true);
		// dołączamy użytkowników, którym odpowiedziano
		foreach (const Contact &contact, chat.contacts())
			repliedUsers.insert(contact);
	}

	kdebugf2();
}

void AutoResponder::chatWidgetClosed(ChatWidget *chatWidget)
{
	Chat chat = chatWidget->chat();
	foreach (const Contact &contact, chat.contacts())
		repliedUsers.remove(contact);
}

void AutoResponder::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	autoRespondTextLineEdit = static_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("autoresponder/autoRespondText"));
	autoRespondTextLineEdit->setToolTip(qApp->translate("@default", MainConfigurationWindow::SyntaxText));
}

void AutoResponder::configurationUpdated()
{
	kdebugf();

	autoRespondText = config_file.readEntry("Autoresponder", "Autotext");

	respondConferences = config_file.readBoolEntry("Autoresponder", "RespondConf");
	respondOnlyFirst = config_file.readBoolEntry("Autoresponder", "OnlyFirstTime");

	statusAvailable = config_file.readBoolEntry("Autoresponder", "StatusAvailable");
	statusBusy = config_file.readBoolEntry("Autoresponder", "StatusBusy");
	statusInvisible = config_file.readBoolEntry("Autoresponder", "StatusInvisible");

	kdebugf2();
}

void AutoResponder::createDefaultConfiguration()
{
	config_file.addVariable("Autoresponder", "Autotext", tr("I am busy."));
	config_file.addVariable("Autoresponder", "OnlyFirstTime", true);
	config_file.addVariable("Autoresponder", "RespondConf", true);
	config_file.addVariable("Autoresponder", "StatusAvailable", false);
	config_file.addVariable("Autoresponder", "StatusBusy", true);
	config_file.addVariable("Autoresponder", "StatusInvisible", false);
}

Q_EXPORT_PLUGIN2(autoresponder, AutoResponder)

/** @} */
