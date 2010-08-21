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

#include <QtGui/QAction>
#include <QtGui/QInputDialog>
#include <QtGui/QLineEdit>
#include <QtGui/QSortFilterProxyModel>
#include <QtCore/QDebug>

#include "identities/identity-manager.h"
#include "identities/model/identity-model.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"

#include "identities-combo-box.h"

IdentitiesComboBox::IdentitiesComboBox(QWidget *parent) :
		QComboBox(parent)
{
	Model = new IdentityModel(this);

	CreateNewIdentityAction = new QAction(tr("Create a new identity..."), this);
	CreateNewIdentityAction->setData("createNewIdentity");

	ActionsModel = new ActionsProxyModel(this);
	ActionsModel->addBeforeAction(new QAction(tr(" - Select identity - "), this));
	ActionsModel->addAfterAction(CreateNewIdentityAction);
	ActionsModel->setSourceModel(Model);

	setModel(ActionsModel);

	connect(this, SIGNAL(activated(int)), this, SLOT(activatedSlot(int)));

	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

IdentitiesComboBox::~IdentitiesComboBox()
{
}

void IdentitiesComboBox::setCurrentIdentity(Identity identity)
{
	QModelIndex index = Model->indexForValue(identity);
	index = ActionsModel->mapFromSource(index);

	if (index.row() < 0 || index.row() >= count())
		setCurrentIndex(0);
	else
		setCurrentIndex(index.row());

	CurrentIdentity = identity;
}

Identity IdentitiesComboBox::currentIdentity()
{
	QModelIndex index = ActionsModel->index(currentIndex(), 0);
	CurrentIdentity = index.data(IdentityRole).value<Identity>();
	return CurrentIdentity;
}

void IdentitiesComboBox::activatedSlot(int index)
{
	QModelIndex modelIndex = this->model()->index(index, 0, QModelIndex());
	QAction *action = modelIndex.data(ActionRole).value<QAction *>();

	if (action != CreateNewIdentityAction)
		return;

	bool ok;

	QString identityName = QInputDialog::getText(this, tr("New Identity"),
			tr("Please enter the name for the new identity:"), QLineEdit::Normal,
			QString::null, &ok);

	if (!ok || identityName.isEmpty() || IdentityManager::instance()->byName(identityName, false))
	{
		setCurrentIndex(0);
		return;
	}

	setCurrentIdentity(IdentityManager::instance()->byName(identityName, true));
}

void IdentitiesComboBox::resetIdentity()
{
	setCurrentIdentity(CurrentIdentity);
}
