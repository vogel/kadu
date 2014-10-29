/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "otr-op-data.h"

#include "otr-error-message-service.h"

const char * OtrErrorMessageService::wrapperOtrErrorMessage(void *data, ConnContext *context, OtrlErrorCode errorCode)
{
	Q_UNUSED(context);

	OtrOpData *opData = static_cast<OtrOpData *>(data);
	if (opData->errorMessageService())
		return strdup(qPrintable(opData->errorMessageService()->errorMessage(opData->contact().display(true), errorCode)));
	else
		return 0;
}

void OtrErrorMessageService::wrapperOtrErrorMessageFree(void *data, const char *errorMessage)
{
	Q_UNUSED(data);

	free(const_cast<char *>(errorMessage));
}

OtrErrorMessageService::OtrErrorMessageService()
{
}

OtrErrorMessageService::~OtrErrorMessageService()
{
}

QString OtrErrorMessageService::errorMessage(const QString &peerDisplay, OtrlErrorCode errorCode) const
{
	switch (errorCode)
	{
		case OTRL_ERRCODE_ENCRYPTION_ERROR:
			return tr("Error occurred during message encryption");
		case OTRL_ERRCODE_MSG_NOT_IN_PRIVATE:
			return tr("You sent encrypted data to %1, who wasn't expecting it").arg(peerDisplay);
		case OTRL_ERRCODE_MSG_UNREADABLE:
			return tr("You transmitted an unreadable encrypted message");
		case OTRL_ERRCODE_MSG_MALFORMED:
			return tr("You transmitted a malformed data message");
		default:
			return QString();
	}
}
