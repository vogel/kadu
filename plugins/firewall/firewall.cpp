/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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

/**

Copyright (C) 2005 by
	Pan Wojtas (Wojtek Sulewski)
	wojciech <  _at_    > sulewski.pl
	gg: 2087202

Na podstawie skryptu TCL autorstwoa Attis'a.
Cz��� kodu (atak flood i emotikonami) na podstawie �aty amd_fanatyka
Przystosowanie do kadu 0.6 -  White Eagle
Nowa funkcjonalnosc - Dorregaray
(szczegoly w zalaczonym pliku Changelog)

**/

#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtWidgets/QMessageBox>

#include "plugins/history/history.h"

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-contact.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "formatted-string/formatted-string-factory.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/search-window.h"
#include "icons/icons-manager.h"
#include "message/message-manager.h"
#include "misc/paths-provider.h"
#include "notification/notification-manager.h"
#include "notification/notification/notification.h"
#include "services/message-filter-service.h"
#include "status/status-container.h"
#include "debug.h"

#include "firewall-notification.h"

#include "firewall.h"

Firewall * Firewall::Instance = 0;

void Firewall::createInstance()
{
	if (!Instance)
		Instance = new Firewall();
}

void Firewall::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

Firewall::Firewall() :
		FloodMessages(0)
{
	kdebugf();

	pattern.setCaseSensitivity(Qt::CaseSensitive);

	createDefaultConfiguration();

	configurationUpdated();

	LastMsg.start();
	LastNotify.start();

	Core::instance()->messageFilterService()->registerMessageFilter(this);

	triggerAllAccountsRegistered();

	connect(Core::instance()->chatWidgetRepository(), SIGNAL(chatWidgetRemoved(ChatWidget *)),
			this, SLOT(chatDestroyed(ChatWidget *)));

	kdebugf2();
}

Firewall::~Firewall()
{
	kdebugf();

	triggerAllAccountsUnregistered();

	Core::instance()->messageFilterService()->unregisterMessageFilter(this);

	kdebugf2();
}

void Firewall::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	CurrentFormattedStringFactory = formattedStringFactory;
}

void Firewall::accountRegistered(Account account)
{
	connect(account, SIGNAL(connected()), this, SLOT(accountConnected()));
}

void Firewall::accountUnregistered(Account account)
{
	disconnect(account, 0, this, 0);
}

bool Firewall::acceptMessage(const Message &message)
{
	switch (message.type())
	{
		case MessageTypeReceived:
			return acceptIncomingMessage(message);
		case MessageTypeSent:
			return acceptOutgoingMessage(message);
		default:
			return true;
	}
}

/**
 * @todo split into several incoming filers
 * @todo extract storing to log files to method method
 * @todo extract notification to separate method
 */
bool Firewall::acceptIncomingMessage(const Message &message)
{
	bool ignore = false;

// emotikony s� sprawdzane nawet przy ��czeniu
	const int min_interval_notify = 2000;

	if (CheckFloodingEmoticons)
	{
		if ((!EmoticonsAllowKnown || message.messageSender().isAnonymous()) && checkEmoticons(message.plainTextContent()))
		{
			ignore = true;
			if (LastNotify.elapsed() > min_interval_notify)
			{
				FirewallNotification::notify(message.messageChat(), message.messageSender(), tr("flooding DoS attack with emoticons!"));

				writeLog(message.messageSender(), message.plainTextContent());

				LastNotify.restart();
			}
			kdebugf2();
			return !ignore;
		}
	}

// atak floodem
	if (checkFlood())
	{
		ignore = true;
		if (LastNotify.elapsed() > min_interval_notify)
		{
			FirewallNotification::notify(message.messageChat(), message.messageSender(), tr("flooding DoS attack!"));

			writeLog(message.messageSender(), message.plainTextContent());

			LastNotify.restart();
		}
		kdebugf2();
		return !ignore;
	}

// ochrona przed anonimami
	if (checkChat(message.messageChat(), message.messageSender(), message.plainTextContent(), ignore))
		ignore = true;

// ochrona przed konferencjami
	if (checkConference(message.messageChat()))
		ignore = true;

// wiadomosc zatrzymana. zapisz do loga i wyswietl dymek
	if (ignore)
	{
		if (message.plainTextContent().length() > 50)
			FirewallNotification::notify(message.messageChat(), message.messageSender(), message.plainTextContent().left(50).append("..."));
		else
			FirewallNotification::notify(message.messageChat(), message.messageSender(), message.plainTextContent());

		writeLog(message.messageSender(), message.plainTextContent());

		if (WriteInHistory && CurrentFormattedStringFactory)
		{
			if (History::instance()->currentStorage())
			{
				Message msg = Message::create();
				msg.setContent(CurrentFormattedStringFactory.data()->fromHtml(message.htmlContent()));
				msg.setType(MessageTypeReceived);
				msg.setReceiveDate(QDateTime::currentDateTime());
				msg.setSendDate(QDateTime::currentDateTime());
				History::instance()->currentStorage()->appendMessage(msg);
			}
		}
	}

	return !ignore;
}

bool Firewall::checkConference(const Chat &chat)
{
	kdebugf();

	if (!IgnoreConferences)
		return false;

	if (chat.contacts().count() < 2)
		return false;

	foreach (const Contact &contact, chat.contacts())
	{
		if (!contact.isAnonymous() || Passed.contains(contact))
		{
			kdebugf2();
 			return false;
		}
	}

	kdebugf2();
	return true;
}

bool Firewall::checkChat(const Chat &chat, const Contact &sender, const QString &message, bool &ignore)
{
	kdebugf();

	if (!CheckChats)
		return false;

	// konferencja
	if (chat.contacts().count() > 1)
	{
		kdebugf2();
 		return false;
	}

	if (!sender.isAnonymous() || Passed.contains(sender))
	{
		kdebugf2();
 		return false;
	}

	if (chat.chatAccount().statusContainer()->status().type() == StatusTypeInvisible && DropAnonymousWhenInvisible)
	{
		writeLog(sender, tr("Chat with anonim silently dropped.\n") + "----------------------------------------------------\n");

		kdebugf2();
 		return true;
	}

	if (IgnoreInvisible)
	{
		if (sender.currentStatus().isDisconnected())
		{
			QDateTime dateTime = chat.chatAccount().property("firewall:firewall-account-connected", QDateTime()).toDateTime();
			if (dateTime.isValid() && dateTime < QDateTime::currentDateTime())
			{
				Protocol *protocol = chat.chatAccount().protocolHandler();
				if (!protocol)
				{
					kdebugf2();
					return false;
				}

				MessageManager::instance()->sendMessage(chat, tr("This message has been generated AUTOMATICALLY!\n\nI'm a busy person and I don't have time for stupid chats with the persons hiding itself. If you want to talk with me change the status to Online or Busy first."), true);
			}

			writeLog(sender, tr("Chat with invisible anonim ignored.\n") + "----------------------------------------------------\n");

			kdebugf2();
			return true;
		}
	}

	if (pattern.exactMatch(message.simplified()))
	{
		Passed.insert(sender);

		if (Confirmation)
		{
			Protocol *protocol = chat.chatAccount().protocolHandler();
			if (!protocol)
			{
				kdebugf2();
				return false;
			}

			MessageManager::instance()->sendMessage(chat, ConfirmationText, true);
		}

		writeLog(sender, tr("User wrote right answer!\n") + "----------------------------------------------------\n");

		ignore = true;

		kdebugf2();
		return false;
	}
	else
	{
		if (LastContact != sender && Search)
		{
			SearchWindow *sd = new SearchWindow(Core::instance()->kaduWindow(),
			                                    BuddyManager::instance()->byContact(sender, ActionCreateAndAdd));
			sd->show();
			sd->firstSearch();

			LastContact = sender;
		}

		kdebugm(KDEBUG_INFO, "%s\n", qPrintable(message));

		QDateTime dateTime = chat.chatAccount().property("firewall:firewall-account-connected", QDateTime()).toDateTime();
		if (dateTime.isValid() && dateTime < QDateTime::currentDateTime())
		{
			Protocol *protocol = chat.chatAccount().protocolHandler();
			if (!protocol)
			{
				kdebugf2();
				return false;
			}

			MessageManager::instance()->sendMessage(chat, ConfirmationQuestion, true);
		}

		kdebugf2();
		return true;
	}
}

bool Firewall::checkFlood()
{
	kdebugf();

	if (!CheckDos)
		return false;

	const unsigned int maxFloodMessages = 15;

	if (LastMsg.restart() >= DosInterval)
	{
		FloodMessages = 0;
		kdebugf2();
		return false;
	}

	if (FloodMessages < maxFloodMessages)
	{
		FloodMessages++;
		kdebugf2();
		return false;
	}

	kdebugf2();
	return true;
}

bool Firewall::checkEmoticons(const QString &message)
{
	kdebugf();

	QStringList emots;
	emots << "<" << ":)" << ":(" << ":-(" << ";)" << ":[" << "wrrrr!" << "niee" << "tiaaa" << ":-)" << ";-)" << ":P" << ":-P" << ";P" << ";-P" << "!!" << "??" << ";(" << ";-(" << ":D" << ":-D" << ";D" << ";-D" << ":O" << ":-O" << ";O" << ";-O" << ":>" << ";>" << ":->" << ";->" << ":|" << ";|" << ":-|" << ";-|" << ":]" << ";]" << ":-]" << ";-]" << ":/" << ";/" << ":-/" << ";-/" << ":*" << ":-*" << ";*" << ";-*" << "]:->";

	int occ = 0;
	foreach (const QString &emot, emots)
		occ += message.count(emot);

	kdebugf2();
	return (occ > MaxEmoticons);
}

void Firewall::accountConnected()
{
	kdebugf();

	Account account(sender());
	if (!account)
		return;

	account.addProperty("firewall:firewall-account-connected", QDateTime::currentDateTime().addMSecs(4000), CustomProperties::NonStorable);

	kdebugf2();
}

void Firewall::chatDestroyed(ChatWidget *chatWidget)
{
	kdebugf();

	foreach (const Contact &contact, chatWidget->chat().contacts())
	{
		if (SecuredTemporaryAllowed.contains(contact.ownerBuddy()))
			SecuredTemporaryAllowed.remove(contact.ownerBuddy());
	}

	kdebugf2();
}

bool Firewall::acceptOutgoingMessage(const Message &message)
{
	foreach (const Contact &contact, message.messageChat().contacts())
	{
		Chat chat = ChatTypeContact::findChat(contact, ActionReturnNull);
		if (!chat)
			continue;

		if (contact.isAnonymous() && Core::instance()->chatWidgetRepository()->widgetForChat(chat))
			Passed.insert(contact);
	}

	if (SafeSending)
	{
		foreach (const Contact &contact, message.messageChat().contacts())
		{
			Buddy buddy = contact.ownerBuddy();

			if (buddy)
			{
				if (!buddy.property("firewall-secured-sending:FirewallSecuredSending", false).toBool())
					return true;
			}

			if (!SecuredTemporaryAllowed.contains(buddy))
			{
				switch (QMessageBox::warning(Core::instance()->chatWidgetRepository()->widgetForChat(message.messageChat()), "Kadu",
						tr("Are you sure you want to send this message?"), tr("&Yes"), tr("Yes and allow until chat closed"), tr("&No"), 2, 2))
				{
						default:
							return false;
						case 0:
							return true;
						case 1:
							SecuredTemporaryAllowed.insert(buddy);
							return true;
				}
			}
		}
	}

	return true;
}

void Firewall::writeLog(const Contact &contact, const QString &message)
{
	kdebugf();

	if (!WriteLog)
		return;

	QFile logFile(LogFilePath);

	if (!logFile.exists() && logFile.open(QIODevice::WriteOnly))
	{
		QTextStream stream(&logFile);
		stream << tr("      DATA AND TIME      ::   ID      :: MESSAGE\n") << "----------------------------------------------------\n";
		logFile.close();
	}

	if (logFile.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QTextStream stream(&logFile);
		stream << QDateTime::currentDateTime().toString() << " :: " << contact.display(true) << " :: " << message << "\n";
		logFile.close();
	}

	kdebugf2();
}

void Firewall::configurationUpdated()
{
	CheckFloodingEmoticons = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Firewall", "dos_emoticons", true);
	EmoticonsAllowKnown = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Firewall", "emoticons_allow_known", false);
	WriteLog = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Firewall", "write_log", true);
	LogFilePath = Application::instance()->configuration()->deprecatedApi()->readEntry("Firewall", "logFile", Application::instance()->pathsProvider()->profilePath() + QLatin1String("firewall.log"));
	CheckDos = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Firewall", "dos", true);
	CheckChats = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Firewall", "chats", true);
	IgnoreConferences = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Firewall", "ignore_conferences", true);
	DropAnonymousWhenInvisible = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Firewall", "drop_anonymous_when_invisible", false);
	IgnoreInvisible = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Firewall", "ignore_invisible", false);
	Confirmation = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Firewall", "confirmation", true);
	ConfirmationText = Application::instance()->configuration()->deprecatedApi()->readEntry("Firewall", "confirmation_text", tr("OK, now say hello, and introduce yourself ;-)"));
	Search = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Firewall", "search", true);
	ConfirmationQuestion = Application::instance()->configuration()->deprecatedApi()->readEntry("Firewall", "question", tr("This message has been generated AUTOMATICALLY!\n\nI'm a busy person and I don't have time for stupid chats. Find another person to chat with. If you REALLY want something from me, simple type \"I want something\" (capital doesn't matter)"));
	WriteInHistory = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Firewall", "write_history", true);
	DosInterval = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Firewall", "dos_interval", 500);
	MaxEmoticons = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Firewall", "emoticons_max", 15);
	SafeSending = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Firewall", "safe_sending", false);

	pattern.setPattern(Application::instance()->configuration()->deprecatedApi()->readEntry("Firewall", "answer", tr("I want something")));
}

void Firewall::createDefaultConfiguration()
{
	//domy�lne powiadamianie dymkiem
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "Firewall_Hints", Application::instance()->configuration()->deprecatedApi()->readEntry("Firewall", "show_hint", "true"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "notification_syntax", Application::instance()->configuration()->deprecatedApi()->readEntry("Firewall", "hint_syntax", tr("%u writes")));
	//domy�lne kolory dymk�w
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "Event_Firewall_fgcolor", Application::instance()->configuration()->deprecatedApi()->readEntry("Firewall", "fg_color", "#000080"));//navy
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "Event_Firewall_bgcolor", Application::instance()->configuration()->deprecatedApi()->readEntry("Firewall", "bg_color", "#add8e6"));//lightblue
	//domy�lne warto�ci zmiennych konfiguracyjnych
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "ignore_conferences", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "search", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "chats", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "question", tr("This message has been generated AUTOMATICALLY!\n\nI'm a busy person and I don't have time for stupid chats. Find another person to chat with. If you REALLY want something from me, simple type \"I want something\" (capital doesn't matter)") );
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "answer", tr("I want something") );
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "confirmation", true );
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "confirmation_text", tr("OK, now say hello, and introduce yourself ;-)") );
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "dos", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "dos_interval", 500);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "dos_emoticons", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "emoticons_max", 15);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "emoticons_allow_known", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "safe_sending", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "write_log", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Firewall", "logFile", Application::instance()->pathsProvider()->profilePath() + QLatin1String("firewall.log"));
}

#include "moc_firewall.cpp"
