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

#include "facebook-depreceated-message.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "windows/message-dialog.h"
#include "icons/icons-manager.h"

FacebookDepreceatedMessage::FacebookDepreceatedMessage(QObject *parent) :
		QObject{parent}
{
}

FacebookDepreceatedMessage::~FacebookDepreceatedMessage()
{
}

void FacebookDepreceatedMessage::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void FacebookDepreceatedMessage::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void FacebookDepreceatedMessage::showIfNotSeen()
{
	auto depreceatedMessageSeen = m_configuration->deprecatedApi()->readBoolEntry("JabberProtocol", "FacebookDepreceatedMessageSeen", false);
	if (depreceatedMessageSeen)
		return;

	m_configuration->deprecatedApi()->writeEntry("JabberProtocol", "FacebookDepreceatedMessageSeen", true);

	auto message = tr(
		"Facebook XMPP support is disabled since 01.05.2015. This means Kadu will be no longer able to connect to "
		"Facebook accounts. You can leave your Facebook accounts configured for chat history or you can remove them from "
		"Kadu using Your Account dialog"
	);

	MessageDialog::show(m_iconsManager->iconByPath(KaduIcon("dialog-information")), tr("Kadu - Facebook"), message);
}

#include "moc_facebook-depreceated-message.cpp"
