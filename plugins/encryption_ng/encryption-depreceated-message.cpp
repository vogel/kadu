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

#include "encryption-depreceated-message.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "plugin/activation/plugin-activation-service.h"
#include "plugin/state/plugin-state-manager.h"
#include "plugin/state/plugin-state-service.h"
#include "plugin/state/plugin-state.h"

EncryptionDepreceatedMessage * EncryptionDepreceatedMessage::m_instance = nullptr;

void EncryptionDepreceatedMessage::createInstance()
{
	m_instance = new EncryptionDepreceatedMessage{};
}

void EncryptionDepreceatedMessage::destroyInstance()
{
	delete m_instance;
	m_instance = nullptr;
}

EncryptionDepreceatedMessage * EncryptionDepreceatedMessage::instance()
{
	return m_instance;
}

EncryptionDepreceatedMessage::EncryptionDepreceatedMessage(QObject *parent) :
		QObject{parent}
{
}

EncryptionDepreceatedMessage::~EncryptionDepreceatedMessage()
{
}

void EncryptionDepreceatedMessage::showIfNotSeen()
{
	auto depreceatedMessageSeen = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("EncryptionNg", "DepreceatedMessageSeen", false);
	if (depreceatedMessageSeen)
		return;

	Application::instance()->configuration()->deprecatedApi()->writeEntry("EncryptionNg", "DepreceatedMessageSeen", true);

	auto isOtrActive = Core::instance()->pluginActivationService()->isActive("encryption_otr");
	auto message = tr(
		"This encryption plugin is depreceated and will be removed in future versions of Kadu. "
		"Use OTR Encryption plugin for compatibility with other instant messenger application."
		"\n\n");
	if (isOtrActive)
		message += tr("OTR Encryption is already active and ready to use.");
	else
		message += tr("Do you want to activate OTR Encryption now?");

	if (isOtrActive)
	{
		MessageDialog::show(KaduIcon("dialog-information"), tr("Encryption"), message);
		return;
	}

	auto dialog = MessageDialog::create(KaduIcon("dialog-information"), tr("Encryption"), message);
	dialog->addButton(QMessageBox::Yes, tr("Enable OTR"));
	dialog->addButton(QMessageBox::No, tr("Do not enable OTR"));

	if (dialog->ask())
	{
		Core::instance()->pluginActivationService()->activatePluginWithDependencies("encryption_otr");
		Core::instance()->pluginStateService()->setPluginState("encryption_otr", PluginState::Enabled);
	}
}
