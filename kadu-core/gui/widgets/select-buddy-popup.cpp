/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QLineEdit>

#include "buddies/buddy-manager.h"
#include "buddies/model/buddies-model.h"
#include "gui/widgets/buddies-list-view.h"
#include "model/roles.h"

#include "select-buddy-popup.h"

SelectBuddyPopup::SelectBuddyPopup(QWidget *parent) :
		BuddiesListWidget(FilterAtBottom, parent)
{
	setWindowFlags(Qt::Popup);

	BuddiesModel *model = new BuddiesModel(this);

	connect(view(), SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
	connect(view(), SIGNAL(buddyActivated(Buddy)), this, SIGNAL(buddySelected(Buddy)));
	connect(view(), SIGNAL(buddyActivated(Buddy)), this, SLOT(close()));

	view()->setItemsExpandable(false);
	view()->setModel(model);
	view()->setRootIsDecorated(false);
	view()->setShowAccountName(false);
}

SelectBuddyPopup::~SelectBuddyPopup()
{
}

void SelectBuddyPopup::show(Buddy buddy)
{
#ifndef Q_WS_MAEMO_5
	nameFilterWidget()->setFocus();
#endif

	view()->selectBuddy(buddy);
	BuddiesListWidget::show();
}

void SelectBuddyPopup::itemClicked(const QModelIndex &index)
{
	close();

	Buddy buddy = index.data(BuddyRole).value<Buddy>();
	if (!buddy)
		return;

	emit buddySelected(buddy);
}
