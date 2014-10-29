/*
 * %kadu copyright begin%
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef OTR_ERROR_MESSAGE_SERVICE_H
#define OTR_ERROR_MESSAGE_SERVICE_H

#include <QtCore/QObject>

extern "C" {
#	include <libotr/proto.h>
#	include <libotr/message.h>
}

class OtrErrorMessageService : public QObject
{
	Q_OBJECT

public:
	static const char * wrapperOtrErrorMessage(void *data, ConnContext *context, OtrlErrorCode errorCode);
	static void wrapperOtrErrorMessageFree(void *data, const char *errorMessage);

	Q_INVOKABLE OtrErrorMessageService();
	virtual ~OtrErrorMessageService();

	QString errorMessage(const QString &peerDisplay, OtrlErrorCode errorCode) const;

};

#endif // OTR_ERROR_MESSAGE_SERVICE_H
