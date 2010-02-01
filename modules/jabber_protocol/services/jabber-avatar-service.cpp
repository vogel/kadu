/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
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

#include "client/jabber-client.h"
#include "jabber-avatar-service.h"
#include "jabber-protocol.h"
#include "server/jabber-avatar-fetcher.h"

#include "utils/vcard-factory.h"
	#include <QtCore/QDebug>
void JabberAvatarService::fetchAvatar(Contact contact)
{
	if (contact.id().isEmpty())
		return;

	JabberAvatarFetcher *avatarFetcher = new JabberAvatarFetcher(contact, this);
	connect(avatarFetcher, SIGNAL(avatarFetched(Contact, const QByteArray &)),
			this, SIGNAL(avatarFetched(Contact, const QByteArray &)));
	avatarFetcher->fetchAvatar();
}

void JabberAvatarService::uploadAvatar(QImage avatar)
{
  	AccountAvatar = avatar;
	JabberProtocol *p = dynamic_cast<JabberProtocol *>(MyAccount.protocolHandler());
	if (!p)
		return;

	connect(p->pepManager(),SIGNAL(itemPublished(const XMPP::Jid&, const QString&, const XMPP::PubSubItem&)),
		this, SLOT(itemPublished(const XMPP::Jid&, const QString&, const XMPP::PubSubItem&)));
	connect(p->pepManager(),SIGNAL(publish_success(const QString&, const XMPP::PubSubItem&)),
		this, SLOT(publish_success(const QString&,const XMPP::PubSubItem&)));
	
	XMPP::Jid jid = XMPP::Jid(MyAccount.id());
	QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        AccountAvatar.save(&buffer, "PNG");
	buffer.close();
	
	QByteArray avatar_data = scaleAvatar(ba);
	QImage avatar_image = QImage::fromData(avatar_data);
	
	if(!avatar_image.isNull()) {
	  qDebug() << "image nie null, lecim";
		// Publish data
		QDomDocument* doc = p->client()->client()->doc();
		QString hash = QCA::Hash("sha1").hashToString(avatar_data);
		QDomElement el = doc->createElement("data");
		el.setAttribute("xmlns","http://www.xmpp.org/extensions/xep-0084.html#ns-data ");
		el.appendChild(doc->createTextNode(QCA::Base64().arrayToString(avatar_data)));
		selfAvatarData_ = avatar_data;
		selfAvatarHash_ = hash;
		p->pepManager()->publish("http://www.xmpp.org/extensions/xep-0084.html#ns-data", XMPP::PubSubItem(hash, el));
	}
	else
	{
		QDomDocument* doc = p->client()->client()->doc();
		QDomElement meta_el =  doc->createElement("metadata");
		meta_el.setAttribute("xmlns","http://www.xmpp.org/extensions/xep-0084.html#ns-metadata");
		meta_el.appendChild(doc->createElement("stop"));
		p->pepManager()->publish("http://www.xmpp.org/extensions/xep-0084.html#ns-metadata", XMPP::PubSubItem("current",meta_el));
	}
}

QByteArray JabberAvatarService::scaleAvatar(const QByteArray& b)
{
	int maxSize = 96;
	QImage i = QImage::fromData(b);
	if (i.isNull()) {
		qWarning("AvatarFactory::scaleAvatar(): Null image (unrecognized format?)");
		return QByteArray();
	}
	else if (i.width() > maxSize || i.height() > maxSize) {
		QImage image = i.scaled(maxSize,maxSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
		QByteArray ba;
		QBuffer buffer(&ba);
		buffer.open(QIODevice::WriteOnly);
		image.save(&buffer, "PNG");
		return ba;
	}
	else {
		return b;
	}
}

void JabberAvatarService::itemPublished(const XMPP::Jid& jid, const QString& n, const XMPP::PubSubItem& item)
{
	if (n == "http://www.xmpp.org/extensions/xep-0084.html#ns-data") {
		if (item.payload().tagName() == "data") {
// 			if (pep_avatars_.contains(jid.bare())) {
// 				pep_avatars_[jid.bare()]->setData(item.id(),item.payload().text());
// 			}
		}
		else {
			qWarning("avatars.cpp: Unexpected item payload");
		}
	}
	else if (n == "http://www.xmpp.org/extensions/xep-0084.html#ns-metadata") {
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
	  	JabberProtocol *p = dynamic_cast<JabberProtocol *>(MyAccount.protocolHandler());
		if (!p)
			return;
		qDebug() << "tera tu sie cos dzieje";
		// Publish metadata
		QDomDocument* doc = p->client()->client()->doc();
		QImage avatar_image = QImage::fromData(selfAvatarData_);
		QDomElement meta_el = doc->createElement("metadata");
		meta_el.setAttribute("xmlns","http://www.xmpp.org/extensions/xep-0084.html#ns-metadata");
		QDomElement info_el = doc->createElement("info");
		info_el.setAttribute("id",selfAvatarHash_);
		info_el.setAttribute("bytes",avatar_image.numBytes());
		info_el.setAttribute("height",avatar_image.height());
		info_el.setAttribute("width",avatar_image.width());
		info_el.setAttribute("type",image2type(selfAvatarData_));
		meta_el.appendChild(info_el);
		p->pepManager()->publish("http://www.xmpp.org/extensions/xep-0084.html#ns-metadata",PubSubItem(selfAvatarHash_,meta_el));
	}
}

/**
void JabberAvatarService::fetchingVCardFinished()
{
  	qDebug() << "vcard jest!";
  	JabberProtocol *p = dynamic_cast<JabberProtocol *>(MyAccount.protocolHandler());
	if (!p)
		return;
		qDebug() << "lecim na szczecin!";
	XMPP::Jid jid = XMPP::Jid(MyAccount.id());
	QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        AccountAvatar.save(&buffer, "PNG");
	buffer.close();

	XMPP::VCard v;// = VCardHandler->vcard();
	v.setFullName("archwimil!");
	v.setPhoto(ba);
		qDebug() << "wysylam vcard!";
			
	if (!ba.isEmpty())
	{
	VCardFactory::instance()->setVCard(p->client()->rootTask(), jid, v, this, SLOT(uploadingVCardFinished()));
	qDebug() << "taa, wysylam...";
	}
	else
	  qDebug() << "nie wysyłam, puste...";
}


void JabberAvatarService::uploadingVCardFinished()
{
  VCardHandler = static_cast<JT_VCard*> (sender());
if (VCardHandler->success()) {
	qDebug() << "udalo sie!";
}
else
  qDebug() << "fail!!! reason: " << VCardHandler->statusString().toLocal8Bit().data();
}

**/
