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

#include "gadu-send-gift-action.h"
#include <gadu-protocol.h>

#include "accounts/account.h"
#include "chat/chat.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "core/application.h"
#include "core/core.h"
#include "file-transfer/file-transfer-direction.h"
#include "file-transfer/file-transfer-handler.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer.h"
#include "file-transfer/gui/file-transfer-can-send-result.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"
#include <url-handlers/url-handler-manager.h>

GaduSendGiftAction::GaduSendGiftAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setName("gaduSendGiftAction");
	setText(tr("Send Gift..."));
	setType(ActionDescription::TypeUser);

	registerAction();
}

GaduSendGiftAction::~GaduSendGiftAction()
{
}

void GaduSendGiftAction::actionInstanceCreated(Action *action)
{
	auto account = action->context()->chat().chatAccount();
	auto gaduProtocolHandler = qobject_cast<GaduProtocol *>(account.protocolHandler());
	if (!gaduProtocolHandler)
		return;

	auto imTokenService = gaduProtocolHandler->gaduIMTokenService();
	if (imTokenService)
		connect(imTokenService, SIGNAL(imTokenChanged(QByteArray)), action, SLOT(checkState()));
}

void GaduSendGiftAction::triggered(QWidget* widget, ActionContext* context, bool toggled)
{
	Q_UNUSED(widget)
	Q_UNUSED(toggled)

	if (!context)
		return;

	auto account = context->chat().chatAccount();
	auto gaduProtocolHandler = qobject_cast<GaduProtocol *>(account.protocolHandler());
	if (!gaduProtocolHandler)
		return;

	auto imTokenService = gaduProtocolHandler->gaduIMTokenService();
	if (!imTokenService || imTokenService->imToken().isEmpty())
		return;

	auto contact = context->contacts().toContact();
	auto mainRedirect = QString{"http%3A%2F%2Fwww.gg.pl%2F%23shop%2F"};
	auto url = QString{"https://login.gg.pl/rd_login?IMToken=%1&redirect_url=%2%3"}.arg(imTokenService->imToken(), mainRedirect, contact.id());
	UrlHandlerManager::instance()->openUrl(url.toUtf8(), true);
}

void GaduSendGiftAction::updateActionState(Action *action)
{
	action->setEnabled(false);

	auto account = action->context()->chat().chatAccount();
	auto gaduProtocolHandler = qobject_cast<GaduProtocol *>(account.protocolHandler());
	if (!gaduProtocolHandler)
		return;

	auto imTokenService = gaduProtocolHandler->gaduIMTokenService();
	if (!imTokenService || imTokenService->imToken().isEmpty())
		return;

	action->setEnabled(true);
}

#include "moc_gadu-send-gift-action.cpp"
