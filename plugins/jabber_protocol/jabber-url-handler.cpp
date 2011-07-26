/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QScopedPointer>
#include <QtGui/QCursor>
#include <QtGui/QMenu>
#include <QtGui/QTextDocument>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "chat/chat-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-widget-manager.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"

#include "jabber-url-handler.h"

JabberUrlHandler::JabberUrlHandler()
{
	// Based on: http://mail.jabber.org/pipermail/xmppwg/2005-February/002261.html
	// (RFC5122 - 3.3, XEP-0147)
	// "(?:xmpp|jabber):" - if we ever need to handle jabber: links

	JabberRegExp = QRegExp("\\b"
	                       "xmpp:"
	                       "(?://([^@ ]+)@([^/?# ]+)/?)?"                 // auth-xmpp
	                       "(?:(?:([^@ ]+)@)?([^/?# ]+)(?:/([^?# ]+))?)?" // path-xmpp
	                       "(?:\\?([^&# ]+)"                              // querytype
	                       "(&[^# ]+)?)?"                                 // pair, will need to be reparsed, later
	                       "(?:#(\\S*))?"                                 // fragment
	                       "\\b"
	);
	// Reparse pair with: "&([^=]+)=([^&]+)"
}

bool JabberUrlHandler::isUrlValid(const QByteArray &url)
{
	if (url == "xmpp:")
		return false;

	return JabberRegExp.exactMatch(QString::fromUtf8(url));
}

void JabberUrlHandler::convertUrlsToHtml(HtmlDocument &document, bool generateOnlyHrefAttr)
{
	Q_UNUSED(generateOnlyHrefAttr)

	for (int i = 0; i < document.countElements(); ++i)
	{
		if (document.isTagElement(i))
			continue;

		QString text = document.elementText(i);
		int index = JabberRegExp.indexIn(text);
		if (index < 0)
			continue;

		unsigned int length = JabberRegExp.matchedLength();
		QString jid = Qt::escape(text.mid(index, length));

		document.splitElement(i, index, length);
		document.setElementValue(i, "<a href=\"" + jid + "\">" + jid + "</a>", true);
	}
}

void JabberUrlHandler::openUrl(const QByteArray &url, bool disableMenu)
{
	QList<Account> jabberAccounts = AccountManager::instance()->byProtocolName("jabber");
	if (!jabberAccounts.count())
		return;

	QString jabberId = QString::fromUtf8(url);
	if (jabberId.startsWith(QLatin1String("jid:")))
	{
		jabberId.remove(0, 3);
		jabberId.remove(QRegExp("/*"));
	}

	if (jabberAccounts.count() == 1 || disableMenu)
	{
		Contact contact = ContactManager::instance()->byId(jabberAccounts[0], jabberId, ActionCreateAndAdd);
		Chat chat = ChatManager::instance()->findChat(ContactSet(contact));
		if (chat)
		{
			ChatWidgetManager::instance()->openPendingMessages(chat, true);
			return;
		}
	}
	else
	{
		QScopedPointer<QMenu> menu(new QMenu());

		QStringList ids;
		foreach (const Account &account, jabberAccounts)
		{
			ids.clear();
			ids.append(account.id());
			ids.append(jabberId);

			menu->addAction(account.data()->statusIcon().icon(), account.id())->setData(ids);
		}

		connect(menu.data(), SIGNAL(triggered(QAction *)), this, SLOT(accountSelected(QAction *)));

		menu->exec(QCursor::pos());
	}
}

void JabberUrlHandler::accountSelected(QAction *action)
{
	QStringList ids = action->data().toStringList();

	if (ids.count() != 2)
		return;

	Account account = AccountManager::instance()->byId("jabber", ids[0]);
	if (!account)
		return;

	Contact contact = ContactManager::instance()->byId(account, ids[1], ActionCreateAndAdd);
	Chat chat = ChatManager::instance()->findChat(ContactSet(contact));
	if (chat)
		ChatWidgetManager::instance()->openPendingMessages(chat, true);
}
