/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-manager.h"
#include "jabber-protocol.h"

#include "jabber-vcard-downloader.h"

JabberVCardDownloader::JabberVCardDownloader(Account account, QObject *parent) :
		QObject(parent), MyAccount(account)
{
}

JabberVCardDownloader::~JabberVCardDownloader()
{
}
/*
void JabberVCardDownloader::done(VCard vCard)
{
	emit vCardDownloaded(true, vCard);
	deleteLater();
}
*/
void JabberVCardDownloader::failed()
{
//	emit vCardDownloaded(false, VCard());
	deleteLater();
}

void JabberVCardDownloader::taskFinished()
{/*
	if (!Task || !Task->success())
		failed();
	else
		done(Task->vcard());*/
}

void JabberVCardDownloader::downloadVCard(const QString &id)
{
	Q_UNUSED(id);
	/*
	JabberProtocol *protocol = qobject_cast<JabberProtocol *>(MyAccount.protocolHandler());
	if (!MyAccount || !protocol || !protocol->isConnected() || !protocol->xmppClient())
	{
		failed();
		return;
	}

	Task = new JT_VCard(XmppClient->rootTask());

	connect(Task.data(), SIGNAL(finished()), this, SLOT(taskFinished()));
	Task->get(id);
	Task->go(true);*/
}

#include "moc_jabber-vcard-downloader.cpp"
