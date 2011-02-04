/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "utils/pep-manager.h"
#include "jabber-protocol.h"

#include "jabber-avatar-pep-uploader.h"

#define XMLNS_METADATA "urn:xmpp:avatar:metadata"
#define XMLNS_DATA "urn:xmpp:avatar:data"

JabberAvatarPepUploader::JabberAvatarPepUploader(Account account, QObject *parent) :
		QObject(parent), MyAccount(account)
{
	MyProtocol = qobject_cast<JabberProtocol *>(account.protocolHandler());

	connect(MyProtocol->client(),SIGNAL(publishSuccess(const QString&, const XMPP::PubSubItem&)),
		this, SLOT(publishSuccess(const QString&,const XMPP::PubSubItem&)));
	connect(MyProtocol->client(),SIGNAL(publishError(const QString&, const XMPP::PubSubItem&)),
		this, SLOT(publishError(const QString&,const XMPP::PubSubItem&)));
}

JabberAvatarPepUploader::~JabberAvatarPepUploader()
{
}

void JabberAvatarPepUploader::publishSuccess(const QString &ns, const XMPP::PubSubItem &item)
{
	if ((XMLNS_DATA != ns && XMLNS_METADATA != ns) || item.id() != ItemId)
		return; // not our data

	if (UploadedAvatar.isNull()) // avatar was removed
	{
		emit avatarUploaded(true);

		deleteLater();
		return;
	}

	QDomDocument *doc = MyProtocol->client()->client()->doc();

	QDomElement metaElement = doc->createElement("metadata");
	metaElement.setAttribute("xmlns", XMLNS_METADATA);

	QDomElement infoElement = doc->createElement("info");
	infoElement.setAttribute("id", ItemId);
	infoElement.setAttribute("bytes", UploadedAvatar.numBytes());
	infoElement.setAttribute("height", UploadedAvatar.height());
	infoElement.setAttribute("width", UploadedAvatar.width());
	infoElement.setAttribute("type", "image/png");
	metaElement.appendChild(infoElement);

	MyProtocol->client()->pepManager()->publish(XMLNS_METADATA, XMPP::PubSubItem(ItemId, metaElement));

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
	QDomDocument *doc = MyProtocol->client()->client()->doc();

	QString hash = QCA::Hash("sha1").hashToString(data);

	QDomElement el = doc->createElement("data");
	el.setAttribute("xmlns", XMLNS_DATA);
	el.appendChild(doc->createTextNode(QCA::Base64().arrayToString(data)));

	ItemId = hash;

	MyProtocol->client()->pepManager()->publish(XMLNS_DATA, XMPP::PubSubItem(hash, el));
}

void JabberAvatarPepUploader::doRemove()
{
	QDomDocument *doc = MyProtocol->client()->client()->doc();

	ItemId = "current";

	QDomElement metaDataElement =  doc->createElement("metadata");
	metaDataElement.setAttribute("xmlns", XMLNS_METADATA);
	metaDataElement.appendChild(doc->createElement("stop"));
	MyProtocol->client()->pepManager()->publish(XMLNS_METADATA, XMPP::PubSubItem("current", metaDataElement));
}

void JabberAvatarPepUploader::uploadAvatar(const QImage &avatar, const QByteArray &data)
{
	UploadedAvatar = avatar;

	if (!UploadedAvatar.isNull())
		doUpload(data);
	else
		doRemove();
}
