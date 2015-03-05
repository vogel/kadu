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

#include "services/jabber-error-service.h"
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

void JabberChangePassword::setErrorService(JabberErrorService *errorService)
{
	m_errorService = errorService;
}

void JabberChangePassword::iqReceived(const QXmppIq &iq)
{
	if (iq.id() != m_id)
		return;

	if (m_errorService->isErrorIq(iq))
	{
		auto conditionString = QString{};
		switch (iq.error().condition())
		{
			case QXmppStanza::Error::NotAuthorized:
				conditionString = tr("Current connection is not safe for password change. Use encrypted connection or change password on provider's site.");
				break;
			case QXmppStanza::Error::NotAllowed:
			case QXmppStanza::Error::FeatureNotImplemented:
				conditionString = tr("Password change is not allowed.");
			default:
				break;
		}
		emit error(m_errorService->errorMessage(iq, conditionString));
	}
	else if (iq.type() == QXmppIq::Type::Result)
		emit passwordChanged();

	deleteLater();
}

#include "moc_jabber-change-password.cpp"
