/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "services/jabber-pep-service.h"
#include "jabber-protocol.h"

#include "jabber-avatar-pep-uploader.h"

#define XMLNS_METADATA "urn:xmpp:avatar:metadata"
#define XMLNS_DATA "urn:xmpp:avatar:data"

JabberAvatarPepUploader * JabberAvatarPepUploader::createForAccount(const Account &account, QObject *parent)
{
	XMPP::JabberProtocol *protocol = qobject_cast<XMPP::JabberProtocol *>(account.protocolHandler());
	if (!protocol)
		return 0;
	else
		return new JabberAvatarPepUploader(protocol->xmppClient(), protocol->pepService(), parent);
}

JabberAvatarPepUploader::JabberAvatarPepUploader(XMPP::Client *xmppClient, JabberPepService *pepService, QObject *parent) :
		QObject(parent), XmppClient(xmppClient), PepService(pepService)
{
	if (PepService)
	{
		connect(PepService.data(), SIGNAL(publishSuccess(const QString &, const XMPP::PubSubItem &)),
			this, SLOT(publishSuccess(const QString &, const XMPP::PubSubItem &)));
		connect(PepService.data(), SIGNAL(publishError(const QString &, const XMPP::PubSubItem &)),
			this, SLOT(publishError(const QString &, const XMPP::PubSubItem &)));
	}
}

JabberAvatarPepUploader::~JabberAvatarPepUploader()
{
}

void JabberAvatarPepUploader::publishSuccess(const QString &ns, const XMPP::PubSubItem &item)
{
	if ((XMLNS_DATA != ns && XMLNS_METADATA != ns) || item.id() != ItemId)
		return; // not our data

	if (UploadedAvatar.isNull() || !XmppClient || !PepService) // avatar was removed
	{
		emit avatarUploaded(true);

		deleteLater();
		return;
	}

	QDomDocument *doc = XmppClient.data()->doc();

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

	emit avatarUploaded(true);

	deleteLater();

}

void JabberAvatarPepUploader::publishError(const QString &ns, const XMPP::PubSubItem &item)
{
	Q_UNUSED(ns)
	Q_UNUSED(item)

	emit avatarUploaded(false);

	deleteLater();
}

void JabberAvatarPepUploader::doUpload(const QByteArray &data)
{
	if (!XmppClient || !PepService)
		return;

	QDomDocument *doc = XmppClient.data()->doc();

	QString hash = QCA::Hash("sha1").hashToString(data);

	QDomElement el = doc->createElement("data");
	el.setAttribute("xmlns", XMLNS_DATA);
	el.appendChild(doc->createTextNode(QCA::Base64().arrayToString(data)));

	ItemId = hash;

	PepService.data()->publish(XMLNS_DATA, XMPP::PubSubItem(hash, el));
}

void JabberAvatarPepUploader::doRemove()
{
	if (!XmppClient || !PepService)
		return;

	QDomDocument *doc = XmppClient.data()->doc();

	ItemId = "current";

	QDomElement metaDataElement =  doc->createElement("metadata");
	metaDataElement.setAttribute("xmlns", XMLNS_METADATA);
	metaDataElement.appendChild(doc->createElement("stop"));

	PepService.data()->publish(XMLNS_METADATA, XMPP::PubSubItem("current", metaDataElement));
}

void JabberAvatarPepUploader::uploadAvatar(const QImage &avatar, const QByteArray &data)
{
	UploadedAvatar = avatar;

	if (!UploadedAvatar.isNull())
		doUpload(data);
	else
		doRemove();
}
