/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QMenu>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "status/status-container.h"

#include "gadu-url-handler.h"

GaduUrlHandler::GaduUrlHandler()
{
	GaduRegExp = QRegExp("\\bgg:(/){0,3}[0-9]{1,12}\\b");
}

GaduUrlHandler::~GaduUrlHandler()
{
}

bool GaduUrlHandler::isUrlValid(const QByteArray &url)
{
	return GaduRegExp.exactMatch(QString::fromUtf8(url));
}

void GaduUrlHandler::openUrl(const QByteArray &url, bool disableMenu)
{
	QVector<Account> gaduAccounts = AccountManager::instance()->byProtocolName("gadu");
	if (gaduAccounts.isEmpty())
		return;

	QString gaduId = QString::fromUtf8(url);
	if (gaduId.startsWith(QLatin1String("gg:")))
	{
		gaduId.remove(0, 3);
		gaduId.remove(QRegExp("/*"));
	}

	if (gaduAccounts.count() == 1 || disableMenu)
	{
		const Contact &contact = ContactManager::instance()->byId(gaduAccounts[0], gaduId, ActionCreateAndAdd);
		const Chat &chat = ChatTypeContact::findChat(contact, ActionCreateAndAdd);
		if (chat)
		{
			Core::instance()->chatWidgetManager()->openChat(chat, OpenChatActivation::Activate);
			return;
		}
	}
	else
	{
		QMenu menu;

		QStringList ids;
		foreach (Account account, gaduAccounts)
		{
			ids.clear();
			ids.append(account.id());
			ids.append(gaduId);

			menu.addAction(account.statusContainer()->statusIcon().icon(), account.id())->setData(ids);
		}

		connect(&menu, SIGNAL(triggered(QAction *)), this, SLOT(accountSelected(QAction *)));

		menu.exec(QCursor::pos());
	}
}

void GaduUrlHandler::accountSelected(QAction *action)
{
	QStringList ids = action->data().toStringList();

	if (ids.count() != 2)
		return;

	Account account = AccountManager::instance()->byId("gadu", ids[0]);
	if (!account)
		return;

	const Contact &contact = ContactManager::instance()->byId(account, ids[1], ActionCreateAndAdd);
	const Chat &chat = ChatTypeContact::findChat(contact, ActionCreateAndAdd);
	Core::instance()->chatWidgetManager()->openChat(chat, OpenChatActivation::Activate);
}

#include "moc_gadu-url-handler.cpp"
