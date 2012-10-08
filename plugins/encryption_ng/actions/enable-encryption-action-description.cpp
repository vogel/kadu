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

#include "gui/actions/action.h"
#include "gui/actions/action-context.h"

#include "encryption-chat-data.h"
#include "encryption-manager.h"
#include "encryption-provider-manager.h"

#include "enable-encryption-action-description.h"

EnableEncryptionActionDescription::EnableEncryptionActionDescription(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypePrivate);
	setName("enableEncryptionAction");
	setIcon(KaduIcon("security-high"));
	setText(tr("Enable Encryption"));
	setCheckable(true);

	registerAction();

	connect(EncryptionProviderManager::instance(), SIGNAL(canEncryptChanged(Chat)),
			this, SLOT(canEncryptChanged(Chat)));
}

EnableEncryptionActionDescription::~EnableEncryptionActionDescription()
{
}

void EnableEncryptionActionDescription::actionTriggered(QAction *sender, bool toggled)
{
	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	Chat chat = action->context()->chat();
	if (!chat)
		return;

	EncryptionManager::instance()->chatEncryption(chat)->setEncrypt(toggled);

	if (!EncryptionManager::instance()->setEncryptionEnabled(action->context()->chat(), toggled) && toggled)
		sender->setChecked(false);
}

void EnableEncryptionActionDescription::updateActionState(Action *action)
{
	Chat chat = action->context()->chat();
	// This is needed beacause we may be called before it is called in EncryptionNgPlugin::init().
	// And EncryptionManager may need EncryptionSetUpActionDescription from its c-tor,
	// so we cannot simply change order in EncryptionNgPlugin::init().
	EncryptionManager::createInstance();
	bool canEncrypt = chat && EncryptionProviderManager::instance()->canEncrypt(chat);
	action->setEnabled(canEncrypt);
	action->setChecked(canEncrypt && EncryptionManager::instance()->chatEncryption(chat)->encrypt());
}

void EnableEncryptionActionDescription::canEncryptChanged(const Chat& chat)
{
	// there is only as much actions as chat windows, so this is not really N^2 when
	// this slot is called for each chat when new encryption implementation is loaded/unloaded
	// so no need to optimize it
	foreach (Action *action, actions())
		if (action->context()->chat() == chat)
			action->checkState();
}
