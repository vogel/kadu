/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

extern "C" {
#	include <libotr/userstate.h>
}

#include "accounts/account.h"
#include "chat/chat.h"
#include "contacts/contact.h"
#include "formatted-string/formatted-string.h"
#include "message/message.h"

#include "encryption-ng-otr-app-ops-wrapper.h"
#include "encryption-ng-otr-user-state-service.h"

#include "encryption-ng-otr-message-filter.h"

EncryptionNgOtrMessageFilter::EncryptionNgOtrMessageFilter(QObject *parent) :
		MessageFilter(parent)
{
}

EncryptionNgOtrMessageFilter::~EncryptionNgOtrMessageFilter()
{
}

void EncryptionNgOtrMessageFilter::setEncryptionNgOtrAppOpsWrapper(EncryptionNgOtrAppOpsWrapper *encryptionNgOtrAppOpsWrapper)
{
	OtrAppOpsWrapper = encryptionNgOtrAppOpsWrapper;
}

void EncryptionNgOtrMessageFilter::setEncryptionNgOtrUserStateService(EncryptionNgOtrUserStateService *encryptionNgOtrUserStateService)
{
	OtrUserStateService = encryptionNgOtrUserStateService;
}

bool EncryptionNgOtrMessageFilter::acceptMessage(const Message &message)
{
	Q_UNUSED(message);

	if (MessageTypeSent == message.type() || OtrAppOpsWrapper.isNull() || OtrUserStateService.isNull())
		return true;

	OtrlUserState userState = OtrUserStateService.data()->forAccount(message.messageChat().chatAccount());

	char *newmessage = 0;
	int otrMessageResult = otrl_message_receiving(userState, OtrAppOpsWrapper.data()->ops(), 0, "2964574", "gadu",
			strdup(message.messageSender().id().toUtf8().data()),
			strdup(message.htmlContent().toUtf8().data()),
			&newmessage, 0, 0, 0);

	free(newmessage);

	return otrMessageResult == 0;
}
