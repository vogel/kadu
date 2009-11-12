/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtNetwork/QHttp>
#include <QtXml/QDomDocument>

#include "accounts/account.h"
#include "buddies/avatar.h"
#include "buddies/avatar-manager.h"
#include "misc/path-conversion.h"

#include "client/jabber-client.h"
#include "jabber-protocol.h"
#include "utils/vcard-factory.h"

#include "jabber-avatar-fetcher.h"

JabberAvatarFetcher::JabberAvatarFetcher(Contact contact, QObject *parent) :
		QObject(parent), MyContact(contact)
{
}

void JabberAvatarFetcher::fetchAvatar()
{
	JabberProtocol *jabberProtocol = dynamic_cast<JabberProtocol *>(MyContact.contactAccount().protocolHandler());
	if (!jabberProtocol || !jabberProtocol->isConnected())
		return;
	VCardFactory::instance()->getVCard(MyContact.id(), jabberProtocol->client()->rootTask(), this, SLOT(receivedVCard()));
}

void JabberAvatarFetcher::receivedVCard()
{
	const VCard* vcard = VCardFactory::instance()->vcard(MyContact.id());
	if (vcard && !vcard->photo().isEmpty()) 
	{
		MyContact.contactAvatar().setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + 7200));
		emit avatarFetched(MyContact, vcard->photo());
	}
	deleteLater();
}
