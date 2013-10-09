/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
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

#include <xmpp_client.h>

#include "server/jabber-avatar-uploader.h"
#include "services/jabber-pep-service.h"
#include "jabber-protocol.h"

#include "jabber-avatar-pep-uploader.h"

#define XMLNS_METADATA "urn:xmpp:avatar:metadata"
#define XMLNS_DATA "urn:xmpp:avatar:data"

JabberAvatarPepUploader::JabberAvatarPepUploader(JabberPepService *pepService, QObject *parent) :
		AvatarUploader(parent), PepService(pepService)
{
	Q_ASSERT(PepService.data());

	connect(PepService.data(), SIGNAL(publishSuccess(const QString &, const XMPP::PubSubItem &)),
		this, SLOT(publishSuccess(const QString &, const XMPP::PubSubItem &)));
	connect(PepService.data(), SIGNAL(publishError(const QString &, const XMPP::PubSubItem &)),
		this, SLOT(publishError(const QString &, const XMPP::PubSubItem &)));
}

JabberAvatarPepUploader::~JabberAvatarPepUploader()
{
}

void JabberAvatarPepUploader::done()
{
	emit avatarUploaded(true, UploadedAvatar);
	deleteLater();
}

void JabberAvatarPepUploader::failed()
{
	emit avatarUploaded(false, UploadedAvatar);
	deleteLater();
}

void JabberAvatarPepUploader::publishSuccess(const QString &ns, const XMPP::PubSubItem &item)
{
	if ((XMLNS_DATA != ns && XMLNS_METADATA != ns) || item.id() != ItemId)
		return; // not our data

	if (!PepService || !PepService.data()->xmppClient())
	{
		failed();
		return;
	}

	if (UploadedAvatar.isNull()) // avatar was removed
	{
		done();
		return;
	}

	QDomDocument *doc = PepService.data()->xmppClient()->doc();

	QDomElement metaElement = doc->createElement("metadata");
	metaElement.setAttribute("xmlns", XMLNS_METADATA);

	QDomElement infoElement = doc->createElement("info");
	infoElement.setAttribute("id", ItemId);
	infoElement.setAttribute("bytes", UploadedAvatar.numBytes());
	infoElement.setAttribute("height", UploadedAvatar.height());
	infoElement.setAttribute("width", UploadedAvatar.width());
	infoElement.setAttribute("type", "image/png");
	metaElement.appendChild(infoElement);

	PepService.data()->publish(XMLNS_METADATA, XMPP::PubSubItem(ItemId, metaElement));

	done();
}

void JabberAvatarPepUploader::publishError(const QString &ns, const XMPP::PubSubItem &item)
{
	if ((XMLNS_DATA != ns && XMLNS_METADATA != ns) || item.id() != ItemId)
		return; // not our data

	failed();
}

void JabberAvatarPepUploader::doUpload(const QByteArray &data)
{
	if (!PepService || !PepService.data()->xmppClient())
		return;

	ItemId = QCA::Hash("sha1").hashToString(data);

	QDomDocument *doc = PepService.data()->xmppClient()->doc();

	QDomElement dataElement = doc->createElement("data");
	dataElement.setAttribute("xmlns", XMLNS_DATA);
	dataElement.appendChild(doc->createTextNode(QCA::Base64().arrayToString(data)));

	PepService.data()->publish(XMLNS_DATA, XMPP::PubSubItem(ItemId, dataElement));
}

void JabberAvatarPepUploader::doRemove()
{
	if (!PepService || !PepService.data()->xmppClient())
		return;

	QDomDocument *doc = PepService.data()->xmppClient()->doc();

	ItemId = "current";

	QDomElement metaDataElement =  doc->createElement("metadata");
	metaDataElement.setAttribute("xmlns", XMLNS_METADATA);
	metaDataElement.appendChild(doc->createElement("stop"));

	PepService.data()->publish(XMLNS_METADATA, XMPP::PubSubItem(ItemId, metaDataElement));
}

void JabberAvatarPepUploader::uploadAvatar(const QString &id, const QString &password, QImage avatar)
{
	Q_UNUSED(id)
	Q_UNUSED(password)

	UploadedAvatar = avatar;

	if (!UploadedAvatar.isNull())
		doUpload(JabberAvatarUploader::avatarData(avatar));
	else
		doRemove();
}

#include "moc_jabber-avatar-pep-uploader.cpp"
