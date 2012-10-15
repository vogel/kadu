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
#include "gui/actions/actions.h"

#include "encryption-chat-data.h"
#include "encryption-manager.h"
#include "encryption-provider-manager.h"

#include "encryption-set-up-menu.h"

EncryptionSetUpMenu::EncryptionSetUpMenu(Action *action, QWidget *parent) :
		QMenu(parent), MenuAction(action)
{
	EncryptorsGroup = new QActionGroup(this);
	EncryptorsGroup->setExclusive(true);

	connect(EncryptorsGroup, SIGNAL(selected(QAction*)), this, SLOT(encryptionMethodSelected(QAction*)));
	connect(this, SIGNAL(aboutToShow()), SLOT(aboutToShowSlot()));
}

EncryptionSetUpMenu::~EncryptionSetUpMenu()
{
}

void EncryptionSetUpMenu::aboutToShowSlot()
{
	clear();

	Chat chat = MenuAction->context()->chat();
	if (!chat)
		return;

	EncryptionProvider *currentEncryptionProvider = EncryptionManager::instance()->encryptionProvider(chat);

	QAction *noEncryption = addAction(tr("No Encryption"));
	noEncryption->setActionGroup(EncryptorsGroup);
	noEncryption->setCheckable(true);
	noEncryption->setChecked(0 == currentEncryptionProvider);

	foreach (EncryptionProvider *encryptionProvider, EncryptionProviderManager::instance()->providers())
	{
		QAction *encryptorAction = addAction(tr("%1 Encryption").arg(encryptionProvider->displayName()));
		encryptorAction->setActionGroup(EncryptorsGroup);
		encryptorAction->setCheckable(true);
		encryptorAction->setChecked(encryptionProvider == currentEncryptionProvider);
		encryptorAction->setData(QVariant::fromValue(encryptionProvider));
	}

	addSeparator();
	addAction(Actions::instance()->createAction("sendPublicKeyAction", MenuAction->context(), MenuAction->parent()));
}

void EncryptionSetUpMenu::encryptionMethodSelected(QAction *selectedAction)
{
	Chat chat = MenuAction->context()->chat();
	if (!chat)
		return;

	EncryptionProvider *encryptionProvider = selectedAction->data().value<EncryptionProvider *>();
	EncryptionManager::instance()->chatEncryption(chat)->setEncrypt(encryptionProvider);

	EncryptionManager::instance()->setEncryptionProvider(chat, encryptionProvider);
}
