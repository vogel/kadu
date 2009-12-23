/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QLineEdit>

#include "buddies/buddy-manager.h"
#include "buddies/model/buddies-model.h"
#include "gui/widgets/buddies-list-view.h"
#include "model/roles.h"

#include "select-buddy-popup.h"

SelectBuddyPopup::SelectBuddyPopup(QWidget *parent) :
		BuddiesListWidget(FilterAtBottom, 0, parent)
{
	setWindowFlags(Qt::Popup);

	BuddiesModel *model = new BuddiesModel(BuddyManager::instance(), this);

	connect(view(), SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
	connect(view(), SIGNAL(buddyActivated(Buddy)), this, SIGNAL(buddySelected(Buddy)));
	connect(view(), SIGNAL(buddyActivated(Buddy)), this, SLOT(close()));

	view()->setItemsExpandable(false);
	view()->setModel(model);
}

SelectBuddyPopup::~SelectBuddyPopup()
{
}

void SelectBuddyPopup::show(Buddy buddy)
{
	nameFilterEdit()->setFocus();

	view()->selectBuddy(buddy);
	BuddiesListWidget::show();
}

void SelectBuddyPopup::itemClicked(const QModelIndex &index)
{
	close();

	QVariant buddyVariant = index.data(BuddyRole);
	if (!buddyVariant.canConvert<Buddy>())
		return;

	Buddy buddy = buddyVariant.value<Buddy>();
	if (!buddy)
		return;

	emit buddySelected(buddy);
}
