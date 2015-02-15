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

#pragma once

#include <QtCore/QString>

enum class GaduDriveSendTicketAckStatus;
enum class GaduDriveSendTicketMode;
enum class GaduDriveSendTicketStatus;

class GaduDriveSendTicket
{

public:
	GaduDriveSendTicket();
	GaduDriveSendTicket(QString ticketId, QString sender, QString recipient, QString fileName,
		qint64 fileSize, double progress, GaduDriveSendTicketAckStatus ackStatus,
		GaduDriveSendTicketMode mode, GaduDriveSendTicketStatus status);

	QString ticketId() const;
	QString sender() const;
	QString recipient() const;
	QString fileName() const;
	qint64 fileSize() const;
	double progress() const;
	GaduDriveSendTicketAckStatus ackStatus() const;
	GaduDriveSendTicketMode mode() const;
	GaduDriveSendTicketStatus status() const;

	bool isValid() const;

private:
	QString m_tickedId;
	QString m_sender;
	QString m_recipient;
	QString m_fileName;
	qint64 m_fileSize;
	double m_progress;
	GaduDriveSendTicketAckStatus m_ackStatus;
	GaduDriveSendTicketMode m_mode;
	GaduDriveSendTicketStatus m_status;

};
