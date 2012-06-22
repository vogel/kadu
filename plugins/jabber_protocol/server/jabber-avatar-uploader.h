/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_AVATAR_UPLOADER_H
#define JABBER_AVATAR_UPLOADER_H

#include <QtCore/QWeakPointer>
#include <QtGui/QImage>

class QNetworkAccessManager;
class QNetworkReply;

namespace XMPP { class JabberVCardService; }

class JabberPepService;

class JabberAvatarUploader : public QObject
{
	Q_OBJECT

	QWeakPointer<JabberPepService> PepService;
	QWeakPointer<XMPP::JabberVCardService> VCardService;

	QString Id;
	QImage UploadingAvatar;

	// http://xmpp.org/extensions/xep-0153.html
	// we dont like too big files
	QImage createScaledAvatar(const QImage &avatarToScale);

	void uploadAvatarPEP();
	void uploadAvatarVCard();

private slots:
	void pepAvatarUploaded(bool ok);
	void avatarUploadedSlot(bool ok);

public:
	static QByteArray avatarData(const QImage &avatar);

	JabberAvatarUploader(JabberPepService *pepService, XMPP::JabberVCardService *vCardService, QObject *parent);
	virtual ~JabberAvatarUploader();

	void uploadAvatar(const QString &id, QImage avatar);

signals:
	void avatarUploaded(bool ok, QImage image);

};

#endif // JABBER_AVATAR_UPLOADER_H
