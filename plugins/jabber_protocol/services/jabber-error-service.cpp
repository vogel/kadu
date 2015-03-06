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

#include "jabber-error-service.h"

#include <qxmpp/QXmppIq.h>
#include <qxmpp/QXmppStanza.h>

JabberErrorService::JabberErrorService(QObject *parent) :
		QObject{parent}
{
}

JabberErrorService::~JabberErrorService()
{
}

bool JabberErrorService::isErrorIq(const QXmppIq &iq) const
{
	return iq.type() == QXmppIq::Type::Error;
}

QString JabberErrorService::errorMessage(QXmppClient *client, QXmppClient::Error error) const
{
	switch (error)
	{
		case QXmppClient::Error::SocketError:
			return tr("Socket error: %1").arg(client->socketErrorString());
		case QXmppClient::Error::KeepAliveError:
			return tr("Server did not respond for PING message");
		case QXmppClient::Error::XmppStreamError:
			return conditionToString(client->xmppStreamError());
		default:
			return QString{};
	}
}

QString JabberErrorService::errorMessage(const QXmppStanza &stanza, QString conditionString) const
{
	if (conditionString.isEmpty())
		conditionString = conditionToString(stanza.error().condition());
	if (conditionString.isEmpty())
		conditionString = tr("Unknown error: %1/%2").arg(stanza.error().condition()).arg(stanza.error().code());

	if (stanza.error().text().isEmpty())
		return conditionString;
	else
		return QString{"%1\nDetails: %2"}.arg(conditionString).arg(stanza.error().text());
}

QString JabberErrorService::conditionToString(QXmppStanza::Error::Condition condition) const
{
	switch (condition)
	{
		case QXmppStanza::Error::Condition::BadRequest:
			return tr("Bad request");
		case QXmppStanza::Error::Condition::Conflict:
			return tr("Conflict");
		case QXmppStanza::Error::Condition::FeatureNotImplemented:
			return tr("Feature not implemented");
		case QXmppStanza::Error::Condition::Forbidden:
			return tr("Forbidden");
		case QXmppStanza::Error::Condition::Gone:
			return tr("Gone");
		case QXmppStanza::Error::Condition::InternalServerError:
			return tr("Internal server error");
		case QXmppStanza::Error::Condition::ItemNotFound:
			return tr("Item not found");
		case QXmppStanza::Error::Condition::JidMalformed:
			return tr("Jid malformed");
		case QXmppStanza::Error::Condition::NotAcceptable:
			return tr("Not acceptable");
		case QXmppStanza::Error::Condition::NotAllowed:
			return tr("Not allowed");
		case QXmppStanza::Error::Condition::NotAuthorized:
			return tr("Not authorized");
		case QXmppStanza::Error::Condition::PaymentRequired:
			return tr("Payment required");
		case QXmppStanza::Error::Condition::RecipientUnavailable:
			return tr("Recipient unavailable");
		case QXmppStanza::Error::Condition::Redirect:
			return tr("Redirect");
		case QXmppStanza::Error::Condition::RegistrationRequired:
			return tr("Registration required");
		case QXmppStanza::Error::Condition::RemoteServerNotFound:
			return tr("Remote server not found");
		case QXmppStanza::Error::Condition::RemoteServerTimeout:
			return tr("Remove server timeout");
		case QXmppStanza::Error::Condition::ResourceConstraint:
			return tr("Resource constraint");
		case QXmppStanza::Error::Condition::ServiceUnavailable:
			return tr("Service unavailable");
		case QXmppStanza::Error::Condition::SubscriptionRequired:
			return tr("Subscription required");
		case QXmppStanza::Error::Condition::UndefinedCondition:
			return tr("Undefined condition");
		case QXmppStanza::Error::Condition::UnexpectedRequest:
			return tr("Unexpected request");
		default:
			return QString{};
	}
}

#include "moc_jabber-error-service.cpp"
