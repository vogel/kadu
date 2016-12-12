/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include <QtGui/QCursor>
#include <QtWidgets/QMenu>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "chat/chat-manager.h"
#include "chat/chat-storage.h"
#include "chat/type/chat-type-contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "status/status-container.h"
#include "widgets/chat-widget/chat-widget-manager.h"

#include "gadu-url-handler.h"

GaduUrlHandler::GaduUrlHandler(QObject *parent) :
		QObject{parent}
{
	m_gaduRegExp = QRegExp("\\bgg:(/){0,3}[0-9]{1,12}\\b");
}

GaduUrlHandler::~GaduUrlHandler()
{
}

void GaduUrlHandler::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void GaduUrlHandler::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void GaduUrlHandler::setChatStorage(ChatStorage *chatStorage)
{
	m_chatStorage = chatStorage;
}

void GaduUrlHandler::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void GaduUrlHandler::setContactManager(ContactManager *contactManager)
{
	m_contactManager = contactManager;
}

void GaduUrlHandler::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

bool GaduUrlHandler::isUrlValid(const QByteArray &url)
{
	return m_gaduRegExp.exactMatch(QString::fromUtf8(url));
}

void GaduUrlHandler::openUrl(UrlOpener *urlOpener, const QByteArray &url, bool disableMenu)
{
	Q_UNUSED(urlOpener);

	auto gaduAccounts = m_accountManager->byProtocolName("gadu");
	if (gaduAccounts.isEmpty())
		return;

	auto gaduId = QString::fromUtf8(url);
	if (gaduId.startsWith(QStringLiteral("gg:")))
	{
		gaduId.remove(0, 3);
		gaduId.remove(QRegExp("/*"));
	}

	if (gaduAccounts.count() == 1 || disableMenu)
	{
		auto const &contact = m_contactManager->byId(gaduAccounts[0], gaduId, ActionCreateAndAdd);
		auto const &chat = ChatTypeContact::findChat(m_chatManager, m_chatStorage, contact, ActionCreateAndAdd);
		if (chat)
		{
			m_chatWidgetManager->openChat(chat, OpenChatActivation::Activate);
			return;
		}
	}
	else
	{
		QMenu menu;

		QStringList ids;
		for (auto const &account : gaduAccounts)
		{
			ids.clear();
			ids.append(account.id());
			ids.append(gaduId);

			menu.addAction(m_iconsManager->iconByPath(account.statusContainer()->statusIcon()), account.id())->setData(ids);
		}

		connect(&menu, SIGNAL(triggered(QAction *)), this, SLOT(accountSelected(QAction *)));

		menu.exec(QCursor::pos());
	}
}

void GaduUrlHandler::accountSelected(QAction *action)
{
	auto ids = action->data().toStringList();

	if (ids.count() != 2)
		return;

	auto account = m_accountManager->byId(QStringLiteral("gadu"), ids[0]);
	if (!account)
		return;

	const Contact &contact = m_contactManager->byId(account, ids[1], ActionCreateAndAdd);
	const Chat &chat = ChatTypeContact::findChat(m_chatManager, m_chatStorage, contact, ActionCreateAndAdd);
	m_chatWidgetManager->openChat(chat, OpenChatActivation::Activate);
}

#include "moc_gadu-url-handler.cpp"
