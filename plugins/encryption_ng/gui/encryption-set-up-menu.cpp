/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "gui/actions/action-context.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/menu/menu-inventory.h"

#include "encryption-chat-data.h"
#include "encryption-manager.h"
#include "encryption-provider-manager.h"

#include "encryption-set-up-menu.h"

EncryptionSetUpMenu::EncryptionSetUpMenu(Action *action, QWidget *parent) :
		QMenu(parent), MenuAction(action)
{
	EncryptorsGroup = new QActionGroup(this);
	EncryptorsGroup->setExclusive(true);

	connect(EncryptorsGroup, SIGNAL(triggered(QAction*)), this, SLOT(encryptionMethodSelected(QAction*)));
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

	KaduMenu *additionalItems = MenuInventory::instance()->menu("encryption-ng");
	if (!additionalItems->empty())
	{
		addSeparator();
		additionalItems->appendTo(this, MenuAction->context());
		additionalItems->update();
	}
}

void EncryptionSetUpMenu::encryptionMethodSelected(QAction *selectedAction)
{
	Chat chat = MenuAction->context()->chat();
	if (!chat)
		return;

	EncryptionProvider *encryptionProvider = selectedAction->data().value<EncryptionProvider *>();
	EncryptionManager::instance()->chatEncryption(chat)->setEncrypt(encryptionProvider);

	if (encryptionProvider)
		EncryptionManager::instance()->chatEncryption(chat)->setLastEncryptionProviderName(encryptionProvider->name());

	EncryptionManager::instance()->setEncryptionProvider(chat, encryptionProvider);
}
