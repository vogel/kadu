/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QMenu>

#include "chat/chat.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"

#include "gui/encryption-set-up-menu.h"
#include "encryption-chat-data.h"
#include "encryption-manager.h"
#include "encryption-provider-manager.h"

#include "encryption-set-up-action-description.h"

EncryptionSetUpActionDescription::EncryptionSetUpActionDescription(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeChat);
	setName("encryptionAction");
	setIcon(KaduIcon("security-high"));
	setText(tr("Encrypt"));
	setCheckable(true);

	registerAction();

	connect(EncryptionProviderManager::instance(), SIGNAL(canEncryptChanged(Chat)),
			this, SLOT(canEncryptChanged(Chat)));
}

EncryptionSetUpActionDescription::~EncryptionSetUpActionDescription()
{
	disconnect(EncryptionProviderManager::instance(), 0, this, 0);
}

void EncryptionSetUpActionDescription::actionTriggered(QAction *sender, bool toggled)
{
	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	Chat chat = action->context()->chat();
	if (!chat)
		return;

	EncryptionManager::instance()->chatEncryption(chat)->setEncrypt(toggled);
	if (!toggled)
	{
		EncryptionManager::instance()->setEncryptionProvider(chat, 0);
		sender->setChecked(false);
		return;
	}

	EncryptionProvider *encryptorProvider = EncryptionProviderManager::instance()->defaultEncryptorProvider(chat);
	EncryptionManager::instance()->setEncryptionProvider(chat, encryptorProvider);
	sender->setChecked(encryptorProvider);
}

QMenu * EncryptionSetUpActionDescription::menuForAction(Action* action)
{
	// no parents for menu as it is destroyed manually by Action class
	return new EncryptionSetUpMenu(action);
}

void EncryptionSetUpActionDescription::updateActionState(Action *action)
{
	Chat chat = action->context()->chat();
	// This is needed beacause we may be called before it is called in EncryptionNgPlugin::init().
	// And EncryptionManager may need EncryptionSetUpActionDescription from its c-tor,
	// so we cannot simply change order in EncryptionNgPlugin::init().
	EncryptionManager::createInstance();
	bool canEncrypt = chat && EncryptionProviderManager::instance()->canEncrypt(chat);
	action->setChecked(canEncrypt && EncryptionManager::instance()->chatEncryption(chat)->encrypt());
}

void EncryptionSetUpActionDescription::canEncryptChanged(const Chat &chat)
{
	// there is only as much actions as chat windows, so this is not really N^2 when
	// this slot is called for each chat when new encryption implementation is loaded/unloaded
	// so no need to optimize it
	foreach (Action *action, actions())
		if (action->context()->chat() == chat)
			action->checkState();
}


QToolButton::ToolButtonPopupMode EncryptionSetUpActionDescription::buttonPopupMode() const
{
	return QToolButton::MenuButtonPopup;
}

#include "moc_encryption-set-up-action-description.cpp"
