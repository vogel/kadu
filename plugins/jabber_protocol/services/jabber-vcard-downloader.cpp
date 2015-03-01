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

#include "jabber-vcard-downloader.h"

#include "jabber-protocol.h"

#include <qxmpp/QXmppVCardIq.h>
#include <qxmpp/QXmppVCardManager.h>

JabberVCardDownloader::JabberVCardDownloader(QXmppVCardManager *vcardManager, QObject *parent) :
		QObject(parent),
		m_vcardManager{vcardManager}
{
}

JabberVCardDownloader::~JabberVCardDownloader()
{
}

void JabberVCardDownloader::done(const QXmppVCardIq &vcard)
{
	emit vCardDownloaded(true, vcard);
	deleteLater();
}

void JabberVCardDownloader::failed()
{
	emit vCardDownloaded(false, QXmppVCardIq{});
	deleteLater();
}

void JabberVCardDownloader::downloadVCard(const QString &id)
{
	m_requestId = m_vcardManager->requestVCard(id);
	if (m_requestId.isEmpty())
		failed();

	connect(m_vcardManager, SIGNAL(vCardReceived(QXmppVCardIq)), this, SLOT(vCardReceived(QXmppVCardIq)));
}

void JabberVCardDownloader::vCardReceived(const QXmppVCardIq &vcard)
{
	if (vcard.id() != m_requestId)
		return;

	done(vcard);
}

#include "moc_jabber-vcard-downloader.cpp"
