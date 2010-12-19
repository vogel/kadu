/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"

#include "encryption-manager.h"
#include "encryption-provider-manager.h"

#include "encryption-actions.h"

static void checkCanEncrypt(Action *action)
{
	Chat chat = action->chat();
	if (!chat)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(EncryptionProviderManager::instance()->canEncrypt(chat));
}

EncryptionActions * EncryptionActions::Instance = 0;

void EncryptionActions::registerActions()
{
	if (!Instance)
		Instance = new EncryptionActions();
}

void EncryptionActions::unregisterActions()
{
	delete Instance;
	Instance = 0;
}

EncryptionActions::EncryptionActions()
{
	EnableEncryptionActionDescription = new ActionDescription(this,
			ActionDescription::TypeChat, "encryptionAction",
			this, SLOT(enableEncryptionActionActivated(QAction *, bool)),
			"security-high", "security-high", tr("Encrypt"),
			true, tr("Encrypt"),
			checkCanEncrypt
	);

	connect(EncryptionProviderManager::instance(), SIGNAL(canDecryptChanged(Chat)), this, SLOT(canDecryptChanged(Chat)));
}

EncryptionActions::~EncryptionActions()
{
	disconnect(EncryptionProviderManager::instance(), SIGNAL(canDecryptChanged(Chat)), this, SLOT(canDecryptChanged(Chat)));
}

void EncryptionActions::canDecryptChanged(const Chat &chat)
{
	// there is only as much actions as chat windows, so this is not really N^2 when
	// this slot is called for each chat when new encryption implementation is loaded/unloaded
	// so no need to optimize it
	foreach (Action *action, EnableEncryptionActionDescription->actions())
		if (action->chat() == chat)
			action->checkState();
}

void EncryptionActions::enableEncryptionActionActivated(QAction *sender, bool toggled)
{
	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	if (!action->chat())
		return;

	EncryptionManager::instance()->setEncryptionEnabled(action->chat(), toggled);
}
