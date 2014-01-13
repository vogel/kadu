/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>

#include "debug.h"

#include "gadu-contact-details.h"

#include "gadu-protocol-helper.h"

#define GG_STATUS_INVISIBLE2 0x0009
#define GG8_DESCRIPTION_MASK 0x00ff

StatusType GaduProtocolHelper::statusTypeFromGaduStatus(unsigned int index)
{
	switch (index & GG8_DESCRIPTION_MASK)
	{
		case GG_STATUS_FFC_DESCR:
		case GG_STATUS_FFC:
			return StatusTypeFreeForChat;

		case GG_STATUS_AVAIL_DESCR:
		case GG_STATUS_AVAIL:
			return StatusTypeOnline;

		case GG_STATUS_BUSY_DESCR:
		case GG_STATUS_BUSY:
			return StatusTypeAway;

		case GG_STATUS_DND_DESCR:
		case GG_STATUS_DND:
			return StatusTypeDoNotDisturb;

		case GG_STATUS_INVISIBLE_DESCR:
		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE2:
			return StatusTypeInvisible;

		case GG_STATUS_BLOCKED:
		case GG_STATUS_NOT_AVAIL_DESCR:
		case GG_STATUS_NOT_AVAIL:

		default:
			return StatusTypeOffline;
	}
}

bool GaduProtocolHelper::isBlockingStatus(unsigned int index)
{
	return GG_STATUS_BLOCKED == index;
}

unsigned int GaduProtocolHelper::gaduStatusFromStatus(const Status &status)
{
	bool hasDescription = !status.description().isEmpty();
	StatusType type = status.type();

	if (StatusTypeFreeForChat == type)
		return hasDescription ? GG_STATUS_FFC_DESCR : GG_STATUS_FFC;

	if (StatusTypeOnline == type)
		return hasDescription ? GG_STATUS_AVAIL_DESCR : GG_STATUS_AVAIL;

	if (StatusTypeAway == type || StatusTypeNotAvailable == type)
		return hasDescription ? GG_STATUS_BUSY_DESCR : GG_STATUS_BUSY;

	if (StatusTypeDoNotDisturb == type)
		return hasDescription ? GG_STATUS_DND_DESCR : GG_STATUS_DND;

	if (StatusTypeInvisible == type)
		return hasDescription ? GG_STATUS_INVISIBLE_DESCR : GG_STATUS_INVISIBLE;

	return hasDescription ? GG_STATUS_NOT_AVAIL_DESCR : GG_STATUS_NOT_AVAIL;
}

QString GaduProtocolHelper::connectionErrorMessage(GaduProtocol::GaduError error)
{
	switch (error)
	{
		case GaduProtocol::ConnectionServerNotFound:
			return QCoreApplication::translate("@default", "Unable to connect, server has not been found");
		case GaduProtocol::ConnectionCannotConnect:
			return QCoreApplication::translate("@default", "Unable to connect");
		case GaduProtocol::ConnectionNeedEmail:
			return QCoreApplication::translate("@default", "Please change your email in \"Change password / email\" window. Leave new password field blank.");
		case GaduProtocol::ConnectionInvalidData:
			return QCoreApplication::translate("@default", "Unable to connect, server has returned unknown data");
		case GaduProtocol::ConnectionCannotRead:
			return QCoreApplication::translate("@default", "Unable to connect, connection break during reading");
		case GaduProtocol::ConnectionCannotWrite:
			return QCoreApplication::translate("@default", "Unable to connect, connection break during writing");
		case GaduProtocol::ConnectionIncorrectPassword:
			return QCoreApplication::translate("@default", "Unable to connect, invalid password");
		case GaduProtocol::ConnectionTlsError:
			return QCoreApplication::translate("@default", "Unable to connect, error of negotiation TLS");
		case GaduProtocol::ConnectionIntruderError:
			return QCoreApplication::translate("@default", "Too many connection attempts with bad password!");
		case GaduProtocol::ConnectionUnavailableError:
			return QCoreApplication::translate("@default", "Unable to connect, servers are down");
		case GaduProtocol::ConnectionUnknow:
			return QCoreApplication::translate("@default", "Connection broken");
		case GaduProtocol::ConnectionTimeout:
			return QCoreApplication::translate("@default", "Connection timeout!");
		case GaduProtocol::Disconnected:
			return QCoreApplication::translate("@default", "Disconnection has occurred");
		default:
			kdebugm(KDEBUG_ERROR, "Unhandled error? (%d)\n", int(error));
			return QCoreApplication::translate("@default", "Connection broken");
	}
}

bool GaduProtocolHelper::isConnectionErrorFatal(GaduProtocol::GaduError error)
{
	switch (error)
	{
		case GaduProtocol::ConnectionNeedEmail:
		case GaduProtocol::ConnectionIncorrectPassword:
		case GaduProtocol::ConnectionIntruderError:
			return true;
		default:
			return false;
	}
}

Buddy GaduProtocolHelper::searchResultToBuddy(Account account, gg_pubdir50_t res, int number)
{
	Buddy result = Buddy::create();

	Contact contact = Contact::create();
	contact.setContactAccount(account);
	contact.setOwnerBuddy(result);
	contact.setId(gg_pubdir50_get(res, number, GG_PUBDIR50_UIN));

	const char *pubdirStatus = gg_pubdir50_get(res, number, GG_PUBDIR50_STATUS);
	if (pubdirStatus)
	{
		Status status;
		status.setType(GaduProtocolHelper::statusTypeFromGaduStatus(atoi(pubdirStatus) & 127));
		contact.setCurrentStatus(status);
	}

	result.setFirstName(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_FIRSTNAME)));
	result.setLastName(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_LASTNAME)));
	result.setNickName(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_NICKNAME)));
	result.setBirthYear(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_BIRTHYEAR)).toUShort());
	result.setCity(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_CITY)));
	result.setFamilyName(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_FAMILYNAME)));
	result.setFamilyCity(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_FAMILYCITY)));
	result.setGender((BuddyGender)QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_GENDER)).toUShort());

	return result;
}

unsigned int GaduProtocolHelper::uin(Contact contact)
{
	GaduContactDetails *data = GaduProtocolHelper::gaduContactDetails(contact);
	return data
			? data->uin()
			: 0;
}

GaduContactDetails * GaduProtocolHelper::gaduContactDetails(Contact contact)
{
	if (contact.isNull())
		return 0;
	return dynamic_cast<GaduContactDetails *>(contact.details());
}
