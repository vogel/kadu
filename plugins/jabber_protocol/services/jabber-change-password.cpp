/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-change-password.h"

#include "jid.h"

#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppRegisterIq.h>

JabberChangePassword::JabberChangePassword(const QString &jid, const QString &newPassword, QXmppClient *client, QObject *parent) :
		QObject{parent}
{
	auto registerIq = QXmppRegisterIq{};
	registerIq.setPassword(newPassword);
	registerIq.setType(QXmppIq::Type::Set);
	registerIq.setUsername(Jid::parse(jid).node());

	m_id = registerIq.id();
	client->sendPacket(registerIq);
	connect(client, SIGNAL(iqReceived(QXmppIq)), this, SLOT(iqReceived(QXmppIq)));
}

JabberChangePassword::~JabberChangePassword()
{
}

void JabberChangePassword::iqReceived(const QXmppIq &iq)
{
	if (iq.id() != m_id)
		return;

	if (iq.type() == QXmppIq::Type::Error)
	{
		switch (iq.error().condition())
		{
			case QXmppStanza::Error::NotAuthorized:
				emit error(tr("Current connection is not safe for password change. Use encrypted connection or change password on provider's site."));
				break;
			case QXmppStanza::Error::NotAllowed:
			case QXmppStanza::Error::FeatureNotImplemented:
				emit error(tr("Password change is not allowed."));
				break;
			case QXmppStanza::Error::BadRequest:
			case QXmppStanza::Error::UnexpectedRequest:
			default:
				emit error(tr("Unknown error: %1:%2 %3").arg(iq.error().condition()).arg(iq.error().code()).arg(iq.error().text()));
				break;
		}
	}
	else if (iq.type() == QXmppIq::Type::Result)
		emit passwordChanged();

	deleteLater();
}

#include "moc_jabber-change-password.cpp"
