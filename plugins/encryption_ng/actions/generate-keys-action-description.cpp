/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QMenu>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/menu/menu-inventory.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "identities/identity.h"

#include "keys/key.h"
#include "encryption-manager.h"
#include "key-generator.h"

#include "generate-keys-action-description.h"

GenerateKeysActionDescription::GenerateKeysActionDescription(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeMainMenu);
	setName("encryptionGenerateKeysAction");
	setIcon(KaduIcon("security-high"));
	setText(tr("Generate Encryption Keys"));

	registerAction();

	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)),
			this, SLOT(updateGenerateKeysMenu()));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account)),
			this, SLOT(updateGenerateKeysMenu()));

	MenuInventory::instance()
		->menu("tools")
		->addAction(this, KaduMenu::SectionTools)
		->update();
}

GenerateKeysActionDescription::~GenerateKeysActionDescription()
{
	// actions will delete theirs menus
	MenuInventory::instance()
		->menu("tools")
		->removeAction(this)
		->update();
}

void GenerateKeysActionDescription::actionInstanceCreated(Action *action)
{
	Q_UNUSED(action)

	// It may look like it was suboptimal but in reality there will be
	// only one action instance.
	updateGenerateKeysMenu();
}

void GenerateKeysActionDescription::actionTriggered(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	menuActionTriggered(sender);
}

void GenerateKeysActionDescription::updateGenerateKeysMenu()
{
	foreach (Action *action, actions())
	{
		QMenu *menu = action->menu();
		if (AccountManager::instance()->items().count() < 2)
		{
			delete menu;
			action->setMenu(0);

			if (AccountManager::instance()->items().isEmpty())
			{
				action->setData(QVariant());
				action->setEnabled(false);
			}
			else
			{
				action->setData(QVariant::fromValue(AccountManager::instance()->items().at(0)));
				action->setEnabled(true);
			}
		}
		else
		{
			if (menu)
				menu->clear();
			else
			{
				menu = new QMenu();
				action->setMenu(menu);
				connect(menu, SIGNAL(triggered(QAction*)),
						this, SLOT(menuActionTriggered(QAction*)));
			}

			foreach (const Account &account, AccountManager::instance()->items())
			{
				QAction *menuAction = menu->addAction(QString("%1 (%2)").arg(account.accountIdentity().name(), account.id()));
				menuAction->setData(QVariant::fromValue(account));
			}

			action->setData(QVariant());
			action->setEnabled(true);
		}
	}
}

void GenerateKeysActionDescription::menuActionTriggered(QAction *action)
{
	Account account = action->data().value<Account>();
	if (!account)
		return;

	KeyGenerator *generator = EncryptionManager::instance()->generator();
	if (!generator)
	{
		MessageDialog::show(KaduIcon("dialog-error"), tr("Encryption"), tr("Cannot generate keys. Check if encryption_ng_simlite plugin is loaded"));
		return;
	}

	if (generator->hasKeys(account))
	{
		MessageDialog *dialog = MessageDialog::create(KaduIcon("dialog-information"), tr("Encryption"), tr("Keys already exist. Do you want to overwrite them?"));
		dialog->addButton(QMessageBox::Yes, tr("Overwrite keys"));
		dialog->addButton(QMessageBox::No, tr("Cancel"));

		if (!dialog->ask())
			return;
	}

	if (generator->generateKeys(account))
		MessageDialog::show(KaduIcon("dialog-information"), tr("Encryption"), tr("Keys have been generated"));
	else
		MessageDialog::show(KaduIcon("dialog-error"), tr("Encryption"), tr("Error generating keys"));
}

#include "moc_generate-keys-action-description.cpp"
