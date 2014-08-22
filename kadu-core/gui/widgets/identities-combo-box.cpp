/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QAction>
#include <QtGui/QInputDialog>
#include <QtGui/QLineEdit>

#include "gui/windows/message-dialog.h"
#include "identities/identity-manager.h"
#include "identities/model/identity-model.h"
#include "model/model-chain.h"
#include "model/roles.h"

#include "identities-combo-box.h"

IdentitiesComboBox::IdentitiesComboBox(QWidget *parent) :
		ActionsComboBox(parent)
{
	IdentityManager::instance()->removeUnused();

	CreateNewIdentityAction = new QAction(tr("Create a new identity..."), this);
	QFont createNewIdentityActionFont = CreateNewIdentityAction->font();
	createNewIdentityActionFont.setItalic(true);
	CreateNewIdentityAction->setFont(createNewIdentityActionFont);
	CreateNewIdentityAction->setData(true);
	connect(CreateNewIdentityAction, SIGNAL(triggered()), this, SLOT(createNewIdentity()));
	addAfterAction(CreateNewIdentityAction);

	ModelChain *chain = new ModelChain(this);
	chain->setBaseModel(new IdentityModel(chain));
	setUpModel(IdentityRole, chain);

	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

IdentitiesComboBox::~IdentitiesComboBox()
{
	IdentityManager::instance()->removeUnused();
}

void IdentitiesComboBox::setCurrentIdentity(Identity identity)
{
	setCurrentValue(identity);
}

Identity IdentitiesComboBox::currentIdentity()
{
	return currentValue().value<Identity>();
}

void IdentitiesComboBox::createNewIdentity()
{
	bool ok;

	QString identityName = QInputDialog::getText(this, tr("New Identity"),
			tr("Please enter the name for the new identity:"), QLineEdit::Normal,
			QString(), &ok);

	if (!ok)
		return;

	Identity newIdentity = IdentityManager::instance()->byName(identityName, true);
	if (newIdentity)
		setCurrentIdentity(newIdentity);
}

#include "moc_identities-combo-box.cpp"
