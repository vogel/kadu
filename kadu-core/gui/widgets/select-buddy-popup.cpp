/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/buddy-manager.h"
#include "buddies/model/buddies-model.h"
#include "gui/widgets/buddies-line-edit.h"
#include "gui/widgets/buddies-list-view.h"
#include "model/roles.h"

#include "select-buddy-popup.h"

SelectBuddyPopup::SelectBuddyPopup(QWidget *parent) :
		BuddiesListWidget(FilterAtBottom, 0, parent)
{
	setWindowFlags(Qt::Popup);

	BuddiesModel *model = new BuddiesModel(BuddyManager::instance(), this);

	connect(view(), SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));

	view()->setItemsExpandable(false);
	view()->setModel(model);
}

SelectBuddyPopup::~SelectBuddyPopup()
{
}

void SelectBuddyPopup::show(const QString &text)
{
	nameFilterEdit()->setText(text);
	nameFilterEdit()->setFocus();
	BuddiesListWidget::show();
}

void SelectBuddyPopup::itemClicked(const QModelIndex &index)
{
	printf("item clicked\n");

	close();

	QVariant buddyVariant = index.data(BuddyRole);
	if (!buddyVariant.canConvert<Buddy>())
		return;

	Buddy buddy = buddyVariant.value<Buddy>();
	if (!buddy)
		return;

	printf("buddy emited: %s\n", qPrintable(buddy.display()));
	emit buddySelected(buddy);
}
