/*
 * %kadu copyright begin%
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
	if (!Task || !Task->success())
		failed();
	else
		done(Task->vcard());
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

	Task = new XMPP::JT_VCard(XmppClient->rootTask());

	connect(Task.data(), SIGNAL(finished()), this, SLOT(taskFinished()));
	Task->get(id);
	Task->go(true);
}

#include "moc_jabber-vcard-downloader.cpp"
