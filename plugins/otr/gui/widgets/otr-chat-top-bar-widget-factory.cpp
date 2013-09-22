/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "chat/type/chat-type-manager.h"
#include "contacts/contact-set.h"

#include "gui/widgets/otr-chat-top-bar-widget.h"
#include "otr-app-ops-wrapper.h"
#include "otr-peer-identity-verification-service.h"
#include "otr-trust-level-service.h"

#include "otr-chat-top-bar-widget-factory.h"

void OtrChatTopBarWidgetFactory::setAppOpsWrapper(OtrAppOpsWrapper *otrAppOpsWrapper)
{
	AppOpsWrapper = otrAppOpsWrapper;
}

void OtrChatTopBarWidgetFactory::setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *otrPeerIdentityVerificationService)
{
	PeerIdentityVerificationService = otrPeerIdentityVerificationService;
}

void OtrChatTopBarWidgetFactory::setTrustLevelService(OtrTrustLevelService *trustLevelService)
{
	TrustLevelService = trustLevelService;
}

QWidget * OtrChatTopBarWidgetFactory::createWidget(const Chat &chat, QWidget *parent)
{
	ChatType *chatType = ChatTypeManager::instance()->chatType(chat.type());
	if (chatType->name() != "Contact")
		return 0;

	OtrChatTopBarWidget *result = new OtrChatTopBarWidget(chat.contacts().toContact(), parent);
	result->setTrustLevelService(TrustLevelService.data());

	if (AppOpsWrapper)
	{
		connect(result, SIGNAL(startPrivateConversation(Contact)), AppOpsWrapper.data(), SLOT(startPrivateConversation(Contact)));
		connect(result, SIGNAL(endPrivateConversation(Contact)), AppOpsWrapper.data(), SLOT(endPrivateConversation(Contact)));
	}

	if (PeerIdentityVerificationService)
		connect(result, SIGNAL(verifyPeerIdentity(Contact)), PeerIdentityVerificationService.data(), SLOT(verifyPeerIdentity(Contact)));

	connect(result, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)));
	Widgets.append(result);

	return result;
}

void OtrChatTopBarWidgetFactory::widgetDestroyed(QObject *widget)
{
	OtrChatTopBarWidget *otrWidget = static_cast<OtrChatTopBarWidget *>(widget);
	Widgets.removeAll(otrWidget);
}
