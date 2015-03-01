/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "services/jabber-vcard-downloader.h"
#include "services/jabber-vcard-uploader.h"

#include "jabber-vcard-service.h"

#include <qxmpp/QXmppVCardManager.h>

JabberVCardService::JabberVCardService(QXmppVCardManager *vcardManager, Account account, QObject *parent) :
		QObject{parent},
		m_vcardManager{vcardManager},
		m_account{account}
{
}

JabberVCardService::~JabberVCardService()
{
}

JabberVCardDownloader * JabberVCardService::createVCardDownloader()
{
	return new JabberVCardDownloader(m_vcardManager, m_account, this);
}

JabberVCardUploader * JabberVCardService::createVCardUploader()
{/*
	if (!XmppClient)
		return 0;

	return new JabberVCardUploader(XmppClient.data(), this);*/
	return nullptr;
}

#include "moc_jabber-vcard-service.cpp"
