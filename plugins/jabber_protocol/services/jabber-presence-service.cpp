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

#include "jabber-presence-service.h"

#include "status/status-type.h"
#include "status/status.h"

#include <qxmpp/QXmppPresence.h>

JabberPresenceService::JabberPresenceService(QObject *parent) :
		QObject{parent}
{
}

JabberPresenceService::~JabberPresenceService()
{
}

QXmppPresence JabberPresenceService::statusToPresence(const Status &status)
{
	auto result = QXmppPresence{};
	result.setType(QXmppPresence::Available);
	result.setStatusText(status.description());

	switch (status.type())
	{
		case StatusType::FreeForChat:
			result.setAvailableStatusType(QXmppPresence::Chat);
			break;
		case StatusType::Online:
			result.setAvailableStatusType(QXmppPresence::Online);
			break;
		case StatusType::Away:
			result.setAvailableStatusType(QXmppPresence::Away);
			break;
		case StatusType::NotAvailable:
			result.setAvailableStatusType(QXmppPresence::XA);
			break;
		case StatusType::DoNotDisturb:
			result.setAvailableStatusType(QXmppPresence::DND);
			break;
		case StatusType::Invisible:
			result.setAvailableStatusType(QXmppPresence::DND);
			break;
		case StatusType::Offline:
		default:
			result.setType(QXmppPresence::Unavailable);
			break;
	}

	return result;
}

Status JabberPresenceService::presenceToStatus(const QXmppPresence &presence)
{
	auto status = Status{};
	if (presence.type() == QXmppPresence::Available)
	{
		switch (presence.availableStatusType())
		{
			case QXmppPresence::AvailableStatusType::Online:
				status.setType(StatusType::Online);
				break;
			case QXmppPresence::AvailableStatusType::Away:
				status.setType(StatusType::Away);
				break;
			case QXmppPresence::AvailableStatusType::XA:
				status.setType(StatusType::NotAvailable);
				break;
			case QXmppPresence::AvailableStatusType::DND:
				status.setType(StatusType::DoNotDisturb);
				break;
			case QXmppPresence::AvailableStatusType::Chat:
				status.setType(StatusType::FreeForChat);
				break;
			case QXmppPresence::AvailableStatusType::Invisible:
				status.setType(StatusType::DoNotDisturb);
				break;
		}
	}
	else if (presence.type() == QXmppPresence::Unavailable)
		status.setType(StatusType::Offline);

	status.setDescription(presence.statusText());

	return status;
}

#include "moc_jabber-presence-service.cpp"
