/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2010 Dariusz Markowicz (darom@alari.pl)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "debug.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"
#include "parser/parser.h"

#include "autoresponder.h"

/**
 * @ingroup autoresponder
 * @{
 */
AutoResponder *autoResponder = 0;

extern "C" KADU_EXPORT int autoresponder_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	autoResponder = new AutoResponder();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/autoresponder.ui"));
	MainConfigurationWindow::registerUiHandler(autoResponder);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void autoresponder_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/autoresponder.ui"));
	MainConfigurationWindow::unregisterUiHandler(autoResponder);

	delete autoResponder;
	autoResponder = 0;

	kdebugf2();
}

AutoResponder::AutoResponder(QObject *parent) :
	QObject(parent)
{
	kdebugf();

	triggerAllAccountsRegistered();

	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetOpen(ChatWidget *, bool)),
			this, SLOT(chatOpenedClosed(ChatWidget *, bool)));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget *)),
			this, SLOT(chatOpenedClosed(ChatWidget *)));

	createDefaultConfiguration();
	configurationUpdated();

	kdebugf2();
}

AutoResponder::~AutoResponder()
{
	kdebugf();

	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetOpen(ChatWidget *, bool)),
			this, SLOT(chatOpenedClosed(ChatWidget *, bool)));
	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget *)),
			this, SLOT(chatOpenedClosed(ChatWidget *)));

	kdebugf2();
}

void AutoResponder::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (chatService)
	{
		connect(chatService, SIGNAL(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)),
				this, SLOT(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)));
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
		disconnect(chatService, SIGNAL(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)),
				this, SLOT(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)));
	}
}

void AutoResponder::filterIncomingMessage(Chat chat, Contact sender, QString &message, time_t time, bool &ignore)
{
	Q_UNUSED(time)
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

	// Na chwilę obecną busy == away gdyż:
	// status-type-manager.cpp:
	//   StatusGroup *busy = StatusGroupManager::instance()->statusGroup("Away");
	if ((statusAvailable && protocol->status().group() == "Online")
			|| (statusBusy && protocol->status().group() == "Away")
			|| (statusInvisible && protocol->status().group() == "Invisible"))
	{
		ChatService *chatService = protocol->chatService();
		if (!chatService)
		{
			kdebugf2();
			return;
		}

		chatService->sendMessage(chat, tr("KADU AUTORESPONDER:") + '\n'
				+ Parser::parse(autoRespondText, BuddyOrContact(sender)), true);
		// dołączamy użytkowników, którym odpowiedziano
		foreach (const Contact &contact, chat.contacts())
			repliedUsers.insert(contact);
	}

	kdebugf2();
}

void AutoResponder::chatOpenedClosed(ChatWidget *chatWidget, bool activate)
{
	Q_UNUSED(activate)
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

/** @} */
