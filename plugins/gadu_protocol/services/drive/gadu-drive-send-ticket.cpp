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

#include "gadu-drive-send-ticket.h"

#include "gadu-drive-send-ticket-ack-status.h"
#include "gadu-drive-send-ticket-mode.h"
#include "gadu-drive-send-ticket-status.h"

GaduDriveSendTicket::GaduDriveSendTicket() :
	m_fileSize{0},
	m_progress{0.0},
	m_ackStatus{GaduDriveSendTicketAckStatus::Unknown},
	m_mode{GaduDriveSendTicketMode::Normal},
	m_status{GaduDriveSendTicketStatus::InProgress}
{
}

GaduDriveSendTicket::GaduDriveSendTicket(QString ticketId, QString sender, QString recipient, QString fileName,
	qint64 fileSize, double progress, GaduDriveSendTicketAckStatus ackStatus,
	GaduDriveSendTicketMode mode, GaduDriveSendTicketStatus status) :
		m_tickedId{ticketId},
		m_sender{sender},
		m_recipient{recipient},
		m_fileName{fileName},
		m_fileSize{fileSize},
		m_progress{progress},
		m_ackStatus{ackStatus},
		m_mode{mode},
		m_status{status}
{
}

QString GaduDriveSendTicket::ticketId() const
{
	return m_tickedId;
}

QString GaduDriveSendTicket::sender() const
{
	return m_sender;
}

QString GaduDriveSendTicket::recipient() const
{
	return m_recipient;
}

QString GaduDriveSendTicket::fileName() const
{
	return m_fileName;
}

qint64 GaduDriveSendTicket::fileSize() const
{
	return m_fileSize;
}

double GaduDriveSendTicket::progress() const
{
	return m_progress;
}

GaduDriveSendTicketAckStatus GaduDriveSendTicket::ackStatus() const
{
	return m_ackStatus;
}

GaduDriveSendTicketMode GaduDriveSendTicket::mode() const
{
	return m_mode;
}

GaduDriveSendTicketStatus GaduDriveSendTicket::status() const
{
	return m_status;
}

bool GaduDriveSendTicket::isValid() const
{
	return !m_tickedId.isEmpty();
}
