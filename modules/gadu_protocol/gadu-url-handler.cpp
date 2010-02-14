/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "chat/chat-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-widget-manager.h"
#include "misc/misc.h"

#include "gadu-url-handler.h"

GaduUrlHandler::GaduUrlHandler()
{
	GaduRegExp = QRegExp("gg:(/){0,3}[0-9]{1,8}");
}

bool GaduUrlHandler::isUrlValid(const QString &url)
{
	return GaduRegExp.exactMatch(url);
}

void GaduUrlHandler::convertUrlsToHtml(HtmlDocument &document)
{
	for (int i = 0; i < document.countElements(); ++i)
	{
		if (document.isTagElement(i))
			continue;

		QString text = document.elementText(i);
		int index = GaduRegExp.indexIn(text);
		if (index < 0)
			continue;

		unsigned int length = GaduRegExp.matchedLength();
		QString gg = text.mid(index, length);

		document.splitElement(i, index, length);
		document.setElementValue(i, "<a href=\"" + gg + "\">" + gg + "</a>", true);
	}
}

void GaduUrlHandler::openUrl(const QString &url)
{
	QList<Account> gaduAccounts = AccountManager::instance()->byProtocolName("gadu");
	if (!gaduAccounts.count())
		return;

	QString gaduId = url;
	if (gaduId.startsWith("gg:"))
	{
		gaduId.remove(0, 3);
		gaduId.remove(QRegExp("/*"));
	}

	if (gaduAccounts.count() == 1)
	{
		Contact contact = ContactManager::instance()->byId(gaduAccounts[0], gaduId, ActionCreateAndAdd);
		Chat chat = ChatManager::instance()->findChat(ContactSet(contact));
		if (chat)
		{
			ChatWidgetManager::instance()->openPendingMsgs(chat, true);
			return;
		}
	}
	else
	{
	}
}
