/*
 * %kadu copyright begin%
 * Copyright 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gadu-notify-service.h"

#include "helpers/gadu-protocol-helper.h"
#include "server/gadu-connection.h"
#include "server/gadu-writable-session-token.h"

#include <QtCore/QScopedArrayPointer>
#include <libgadu.h>

int GaduNotifyService::notifyTypeFromContact(const Contact &contact)
{
	if (contact.isAnonymous())
		return 0;

	auto buddy = contact.ownerBuddy();
	auto result = 0x01; // GG_USER_BUDDY
	if (!buddy.isOfflineTo())
		result |= 0x02; // GG_USER_FRIEND
	if (buddy.isBlocked())
		result |= 0x04; // GG_USER_BLOCKED

	return result;
}

GaduNotifyService::GaduNotifyService(GaduConnection *connection, QObject *parent) :
		QObject{parent},
		m_connection{connection}
{
}

GaduNotifyService::~GaduNotifyService()
{
}

void GaduNotifyService::sendInitialData(const QVector<Contact> &contacts)
{
	if (!m_connection || !m_connection->hasSession())
		return;

	auto toSend = QVector<Contact>{};
	for (auto &&contact : contacts)
		if (!contact.isAnonymous())
			toSend.append(contact);

	if (toSend.isEmpty())
	{
		auto writableSessionToken = m_connection->writableSessionToken();
		gg_notify_ex(writableSessionToken.rawSession(), 0, 0, 0);
		return;
	}

	auto count = toSend.count();
	QScopedArrayPointer<UinType> uins{new UinType[count]};
	QScopedArrayPointer<char> types{new char[count]};

	decltype(count) i = 0;

	for (auto &&contact : toSend)
	{
		uins[i] = GaduProtocolHelper::uin(contact);
		types[i] = notifyTypeFromContact(contact);

		contact.addProperty("gadu:flags", types[i], CustomProperties::NonStorable);

		++i;
	}

	auto writableSessionToken = m_connection->writableSessionToken();
	gg_notify_ex(writableSessionToken.rawSession(), uins.data(), types.data(), count);
}

void GaduNotifyService::contactAdded(Contact contact)
{
	sendNewFlags(contact, notifyTypeFromContact(contact));
}

void GaduNotifyService::contactRemoved(Contact contact)
{
	sendNewFlags(contact, 0);
}

void GaduNotifyService::contactUpdatedLocally(Contact contact)
{
	sendNewFlags(contact, notifyTypeFromContact(contact));
}

bool GaduNotifyService::updateFlag(gg_session *session, int uin, int newFlags, int oldFlags, int flag) const
{
	Q_ASSERT(session);

	if (!(oldFlags & flag) && (newFlags & flag))
		return 0 == gg_add_notify_ex(session, uin, flag);
	if ((oldFlags & flag) && !(newFlags & flag))
		return 0 == gg_remove_notify_ex(session, uin, flag);

	return true;
}

void GaduNotifyService::sendNewFlags(const Contact &contact, int newFlags) const
{
	if (!m_connection || !m_connection->hasSession())
		return;

	auto uin = contact.id().toUInt();
	auto oldFlags = contact.property("gadu:flags", 0).toInt();

	if (newFlags == oldFlags)
		return;

	auto writableSessionToken = m_connection->writableSessionToken();
	auto f1 = updateFlag(writableSessionToken.rawSession(), uin, newFlags, oldFlags, 0x01);
	auto f2 = updateFlag(writableSessionToken.rawSession(), uin, newFlags, oldFlags, 0x02);
	auto f3 = updateFlag(writableSessionToken.rawSession(), uin, newFlags, oldFlags, 0x04);

	if (f1 && f2 && f3)
		contact.addProperty("gadu:flags", newFlags, CustomProperties::NonStorable);
}

#include "moc_gadu-notify-service.cpp"
