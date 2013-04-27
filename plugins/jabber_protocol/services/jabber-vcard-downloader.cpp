/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <xmpp_tasks.h>

#include "accounts/account-manager.h"
#include "jabber-protocol.h"

#include "jabber-vcard-downloader.h"

JabberVCardDownloader::JabberVCardDownloader(XMPP::Client *client, QObject *parent) :
		QObject(parent), XmppClient(client)
{
}

JabberVCardDownloader::~JabberVCardDownloader()
{
}

void JabberVCardDownloader::done(XMPP::VCard vCard)
{
	emit vCardDownloaded(true, vCard);
	deleteLater();
}

void JabberVCardDownloader::failed()
{
	emit vCardDownloaded(false, XMPP::VCard());
	deleteLater();
}

void JabberVCardDownloader::taskFinished()
{
	if (!Task || !Task.data()->success())
		failed();
	else
		done(Task.data()->vcard());
}

void JabberVCardDownloader::downloadVCard(const QString &id)
{
	Account account = AccountManager::instance()->byId("jabber", id);

	XMPP::JabberProtocol *protocol = qobject_cast<XMPP::JabberProtocol *>(account.protocolHandler());

	if (account && protocol->isConnected() && protocol->xmppClient())
	{
		failed();
		return;
	}

	Task = new XMPP::JT_VCard(XmppClient.data()->rootTask());

	connect(Task.data(), SIGNAL(finished()), this, SLOT(taskFinished()));
	Task.data()->get(id);
	Task.data()->go(true);
}

#include "moc_jabber-vcard-downloader.cpp"
