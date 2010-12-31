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

#include "gui/windows/message-dialog.h"
#include "identities/identity-manager.h"
#include "identities/model/identity-model.h"
#include "model/roles.h"

#include "identities-combo-box.h"

IdentitiesComboBox::IdentitiesComboBox(bool includeSelectIdentity, QWidget *parent) :
		KaduComboBox<Identity>(parent), InActivatedSlot(false), IncludeSelectIdentity(includeSelectIdentity)
{
	setUpModel(new IdentityModel(this));

	CreateNewIdentityAction = new QAction(tr("Create a new identity..."), this);
	CreateNewIdentityAction->setData("createNewIdentity");

	ActionsModel->addAfterAction(CreateNewIdentityAction);

	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	connect(model(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
			this, SLOT(updateValueBeforeChange()));
	connect(model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
			this, SLOT(rowsRemoved(const QModelIndex &, int, int)));
	connect(this, SIGNAL(activated(int)), this, SLOT(activatedSlot(int)));
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));
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
	return currentValue();
}

void IdentitiesComboBox::resetComboBox()
{
	if (!InActivatedSlot)
		setCurrentIndex(0);
}

void IdentitiesComboBox::activatedSlot(int index)
{
	InActivatedSlot = true;

	QModelIndex modelIndex = this->model()->index(index, modelColumn(), rootModelIndex());
	QAction *action = modelIndex.data(ActionRole).value<QAction *>();

	if (action == CreateNewIdentityAction)
	{
		bool ok;

		QString identityName = QInputDialog::getText(this, tr("New Identity"),
				tr("Please enter the name for the new identity:"), QLineEdit::Normal,
				QString(), &ok);

		Identity identityToSwitch = IdentityManager::instance()->byName(identityName, ok);
		if (identityToSwitch)
			setCurrentIdentity(identityToSwitch);
		else
			setCurrentIndex(0);
	}

	InActivatedSlot = false;
}

void IdentitiesComboBox::currentIndexChangedSlot(int index)
{
	QModelIndex modelIndex = this->model()->index(index, modelColumn(), rootModelIndex());
	QAction *action = modelIndex.data(ActionRole).value<QAction *>();
	if (action == CreateNewIdentityAction)
	{
		// this is needed to fix Mantis bugs #1674 and #1690
		// as this action has to be activated by the user, otherwise we have to ignore it and reset combo box
		// TODO 0.9: try to redo this as this is a bit tricky
		if (!InActivatedSlot)
			QMetaObject::invokeMethod(this, "resetComboBox", Qt::QueuedConnection);
		return;
	}

	if (KaduComboBox<Identity>::currentIndexChangedSlot(index))
		emit identityChanged(CurrentValue);
}

void IdentitiesComboBox::updateValueBeforeChange()
{
	KaduComboBox<Identity>::updateValueBeforeChange();
}

void IdentitiesComboBox::rowsRemoved(const QModelIndex &parent, int start, int end)
{
	KaduComboBox<Identity>::rowsRemoved(parent, start, end);
}

int IdentitiesComboBox::preferredDataRole() const
{
	return IdentityRole;
}

QString IdentitiesComboBox::selectString() const
{
	return IncludeSelectIdentity ? tr(" - Select identity - ") : QString();
}
