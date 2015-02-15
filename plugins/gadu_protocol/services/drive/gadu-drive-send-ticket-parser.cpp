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

#include "gadu-drive-send-ticket-parser.h"

#include "services/drive/gadu-drive-send-ticket-ack-status.h"
#include "services/drive/gadu-drive-send-ticket-mode.h"
#include "services/drive/gadu-drive-send-ticket-status.h"
#include "services/drive/gadu-drive-send-ticket.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

GaduDriveSendTicket GaduDriveSendTicketParser::fromJson(QJsonDocument json)
{
	auto sendTicket = json.object().value("result").toObject().value("send_ticket").toObject();
	auto ticketId = sendTicket.value("id").toString();
	auto sender = sendTicket.value("sender").toString();
	auto recipient = sendTicket.value("recipient").toString();
	auto fileName = sendTicket.value("file_name").toString();
	auto fileSize = sendTicket.value("file_size").toString().toUInt();
	auto progress = sendTicket.value("send_progress").toDouble();
	auto ackStatusString = sendTicket.value("ack_status").toString();
	auto ackStatus = ackStatusString == "allowed"
		? GaduDriveSendTicketAckStatus::Allowed
		: ackStatusString == "rejected"
		? GaduDriveSendTicketAckStatus::Rejected
		: GaduDriveSendTicketAckStatus::Unknown;
	auto modeString = sendTicket.value("send_mode").toString();
	auto mode = modeString == "publink"
		? GaduDriveSendTicketMode::Publink
		: GaduDriveSendTicketMode::Normal;
	auto statusString = sendTicket.value("send_status").toString();
	auto status = statusString == "completed"
		? GaduDriveSendTicketStatus::Completed
		: statusString == "expired"
		? GaduDriveSendTicketStatus::Expired
		: GaduDriveSendTicketStatus::InProgress;

	return GaduDriveSendTicket{std::move(ticketId), std::move(sender), std::move(recipient), std::move(fileName),
		fileSize, progress, ackStatus, mode, status};
}
