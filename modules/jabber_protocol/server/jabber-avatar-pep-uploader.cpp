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

#define NS_METADATA "http://www.xmpp.org/extensions/xep-0084.html#ns-metadata"
#define NS_DATA "http://www.xmpp.org/extensions/xep-0084.html#ns-data"

JabberAvatarPepUploader::JabberAvatarPepUploader(Account account, QObject *parent) :
		QObject(parent), MyAccount(account)
{
	MyProtocol = dynamic_cast<JabberProtocol *>(account.protocolHandler());

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
	if (NS_METADATA != ns || item.id() != UploadedAvatarHash)
		return; // not our data

	QDomDocument *doc = MyProtocol->client()->client()->doc();

	QDomElement metaElement = doc->createElement("metadata");
	metaElement.setAttribute("xmlns", NS_METADATA);

	QDomElement infoElement = doc->createElement("info");
	infoElement.setAttribute("id", UploadedAvatarHash);
	infoElement.setAttribute("bytes", UploadedAvatar.numBytes());
	infoElement.setAttribute("height", UploadedAvatar.height());
	infoElement.setAttribute("width", UploadedAvatar.width());
	infoElement.setAttribute("type", "image/png");
	metaElement.appendChild(infoElement);

	MyProtocol->client()->pepManager()->publish(NS_METADATA, XMPP::PubSubItem(UploadedAvatarHash, metaElement));

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
	el.setAttribute("xmlns", NS_DATA);
	el.appendChild(doc->createTextNode(QCA::Base64().arrayToString(data)));

	UploadedAvatarHash = hash;

	MyProtocol->client()->pepManager()->publish(NS_METADATA, XMPP::PubSubItem(hash, el));
}

void JabberAvatarPepUploader::doRemove()
{
	QDomDocument *doc = MyProtocol->client()->client()->doc();

	QDomElement metaDataElement =  doc->createElement("metadata");
	metaDataElement.setAttribute("xmlns", NS_METADATA);
	metaDataElement.appendChild(doc->createElement("stop"));
	MyProtocol->client()->pepManager()->publish(NS_METADATA, XMPP::PubSubItem("current", metaDataElement));
}

void JabberAvatarPepUploader::uploadAvatar(const QImage &avatar, const QByteArray &data)
{
	UploadedAvatar = avatar;

	if (!UploadedAvatar.isNull())
		doUpload(data);
	else
		doRemove();
}
