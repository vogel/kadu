/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCrypto>

#include "debug.h"
#include "gui/windows/message-dialog.h"

#include "client/jabber-client.h"
#include "jabber-protocol.h"
#include "server/jabber-avatar-fetcher.h"
#include "utils/pep-manager.h"
#include "utils/vcard-factory.h"

#include "jabber-avatar-service.h"

void JabberAvatarService::fetchAvatar(Contact contact)
{
	if (contact.id().isEmpty())
		return;

	Protocol = dynamic_cast<JabberProtocol *>(MyAccount.protocolHandler());
	if (!Protocol)
		return;

	JabberAvatarFetcher *avatarFetcher = new JabberAvatarFetcher(contact, this);
	connect(avatarFetcher, SIGNAL(avatarFetched(Contact, bool, const QByteArray &)),
			this, SIGNAL(avatarFetched(Contact, bool, const QByteArray &)));
	avatarFetcher->fetchAvatar();
}

void JabberAvatarService::uploadAvatar(QImage avatar)
{
  	AccountAvatar = avatar;
	Protocol = dynamic_cast<JabberProtocol *>(MyAccount.protocolHandler());
	if (!Protocol)
		return;

	if (Protocol->isPEPAvailable())
		uploadAvatarPEP();
	else
		uploadAvatarVCard();
}

void JabberAvatarService::uploadAvatarPEP()
{
	connect(Protocol->pepManager(),SIGNAL(itemPublished(const XMPP::Jid&, const QString&, const XMPP::PubSubItem&)),
		this, SLOT(itemPublished(const XMPP::Jid&, const QString&, const XMPP::PubSubItem&)));
	connect(Protocol->pepManager(),SIGNAL(publish_success(const QString&, const XMPP::PubSubItem&)),
		this, SLOT(publish_success(const QString&,const XMPP::PubSubItem&)));
	connect(Protocol->pepManager(),SIGNAL(publish_error(const QString&, const XMPP::PubSubItem&)),
		this, SLOT(publish_error(const QString&,const XMPP::PubSubItem&)));

	XMPP::Jid jid = XMPP::Jid(MyAccount.id());
	QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        AccountAvatar.save(&buffer, "PNG");
	buffer.close();

	QByteArray avatar_data = scaleAvatar(ba);
	QImage avatar_image = QImage::fromData(avatar_data);

	if (!avatar_image.isNull())
	{
		// Publish data
		QDomDocument* doc = Protocol->client()->client()->doc();
		QString hash = QCA::Hash("sha1").hashToString(avatar_data);
		QDomElement el = doc->createElement("data");
		el.setAttribute("xmlns","http://www.xmpp.org/extensions/xep-0084.html#ns-data ");
		el.appendChild(doc->createTextNode(QCA::Base64().arrayToString(avatar_data)));
		selfAvatarData_ = avatar_data;
		selfAvatarHash_ = hash;
		Protocol->pepManager()->publish("http://www.xmpp.org/extensions/xep-0084.html#ns-data", XMPP::PubSubItem(hash, el));
	}
	else
	{
		QDomDocument* doc = Protocol->client()->client()->doc();
		QDomElement meta_el =  doc->createElement("metadata");
		meta_el.setAttribute("xmlns","http://www.xmpp.org/extensions/xep-0084.html#ns-metadata");
		meta_el.appendChild(doc->createElement("stop"));
		Protocol->pepManager()->publish("http://www.xmpp.org/extensions/xep-0084.html#ns-metadata", XMPP::PubSubItem("current",meta_el));
	}
}

void JabberAvatarService::uploadAvatarVCard()
{
	VCardFactory::instance()->getVCard(MyAccount.id(), Protocol->client()->rootTask(), this, SLOT(fetchingVCardFinished()));
}

void JabberAvatarService::fetchingVCardFinished()
{
	XMPP::Jid jid = XMPP::Jid(MyAccount.id());

	QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        AccountAvatar.save(&buffer, "PNG");
	buffer.close();

	XMPP::VCard vcard;
	XMPP::JT_VCard *task = (XMPP::JT_VCard *)sender();

	if (task && task->success())
	{
		vcard = task->vcard();
		vcard.setPhoto(ba);
		VCardFactory::instance()->setVCard(Protocol->client()->rootTask(), jid, vcard, this, SLOT(uploadingVCardFinished()));
	}
	else
		emit avatarUploaded(false, AccountAvatar);
}

void JabberAvatarService::uploadingVCardFinished()
{
	VCardHandler = static_cast<XMPP::JT_VCard *>(sender());
	if (VCardHandler->success())
	{
		emit avatarUploaded(true, AccountAvatar);
	}
	else
	{
		kdebug("VCard upload failed! Reason: %s", VCardHandler->statusString().toLocal8Bit().data());
		emit avatarUploaded(false, AccountAvatar);
	}
}

QByteArray JabberAvatarService::scaleAvatar(const QByteArray& b)
{
	int maxSize = 96;
	QImage i = QImage::fromData(b);
	if (i.isNull())
	{
		qWarning("AvatarFactory::scaleAvatar(): Null image (unrecognized format?)");
		return QByteArray();
	}
	else if (i.width() > maxSize || i.height() > maxSize)
	{
		QImage image = i.scaled(maxSize,maxSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
		QByteArray ba;
		QBuffer buffer(&ba);
		buffer.open(QIODevice::WriteOnly);
		image.save(&buffer, "PNG");
		return ba;
	}
	else
	{
		return b;
	}
}

void JabberAvatarService::itemPublished(const XMPP::Jid& jid, const QString& n, const XMPP::PubSubItem& item)
{
	Q_UNUSED(jid)

	if (n == "http://www.xmpp.org/extensions/xep-0084.html#ns-data")
	{
		if (item.payload().tagName() == "data")
		{
			emit avatarFetched(MyContact, true, item.payload().text().toLatin1());
		}
		else
		{
			qWarning("avatars.cpp: Unexpected item payload");
		}
	}
	else if (n == "http://www.xmpp.org/extensions/xep-0084.html#ns-metadata")
	{
		/*
		if (!pep_avatars_.contains(jid.bare())) {
			pep_avatars_[jid.bare()] = new PEPAvatar(this, jid.bare());
			connect(pep_avatars_[jid.bare()],SIGNAL(avatarChanged(const Jid&)),this, SLOT(updateAvatar(const Jid&)));
		}
		QDomElement e;
		bool found;
		e = findSubTag(item.payload(), "stop", &found);
		if (found) {
			pep_avatars_[jid.bare()]->updateHash("");
		}
		else {
			pep_avatars_[jid.bare()]->updateHash(item.id());
		}
		*/
	}
}

void JabberAvatarService::publish_success(const QString& n, const XMPP::PubSubItem& item)
{
	if (n == "http://www.xmpp.org/extensions/xep-0084.html#ns-data" && item.id() == selfAvatarHash_)
	{
		// Publish metadata
		QDomDocument* doc = Protocol->client()->client()->doc();
		QImage avatar_image = QImage::fromData(selfAvatarData_);
		QDomElement meta_el = doc->createElement("metadata");
		meta_el.setAttribute("xmlns","http://www.xmpp.org/extensions/xep-0084.html#ns-metadata");
		QDomElement info_el = doc->createElement("info");
		info_el.setAttribute("id", selfAvatarHash_);
		info_el.setAttribute("bytes", avatar_image.numBytes());
		info_el.setAttribute("height", avatar_image.height());
		info_el.setAttribute("width", avatar_image.width());
		info_el.setAttribute("type", image2type(selfAvatarData_));
		meta_el.appendChild(info_el);
		Protocol->pepManager()->publish("http://www.xmpp.org/extensions/xep-0084.html#ns-metadata", XMPP::PubSubItem(selfAvatarHash_, meta_el));

		emit avatarUploaded(true, AccountAvatar);
	}
}

void JabberAvatarService::publish_error(const QString& n, const XMPP::PubSubItem& item)
{
	if (n == "http://www.xmpp.org/extensions/xep-0084.html#ns-data" && item.id() == selfAvatarHash_)
	{
		/*
		  Some servers (at least jid.pl were reported) seems to hadle PEP, but each attempt to publish avatar this way
		  ends up with 'PEP not supported' error. In this case we can still try to put avatar into VCard.
		*/
		uploadAvatarVCard();
	}
}




