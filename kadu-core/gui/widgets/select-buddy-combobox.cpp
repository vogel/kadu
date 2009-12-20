/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QListView>

#include "buddies/buddy-manager.h"
#include "buddies/filter/buddy-name-filter.h"
#include "buddies/model/buddies-model.h"
#include "buddies/model/buddies-model-proxy.h"
#include "gui/widgets/buddies-line-edit.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/select-buddy-popup.h"
#include "model/actions-proxy-model.h"

#include "select-buddy-combobox.h"
#include <model/roles.h>

SelectBuddyCombobox::SelectBuddyCombobox(QWidget *parent) :
		QComboBox(parent)
{
	connect(this, SIGNAL(editTextChanged(const QString &)),
			this, SLOT(buddyTextChanged(const QString &)));

	Popup = new SelectBuddyPopup();
	connect(Popup, SIGNAL(buddySelected(Buddy)), this, SLOT(buddySelected(Buddy)));

	Model = new BuddiesModel(BuddyManager::instance(), this);
	ProxyModel = new BuddiesModelProxy(this);
	ProxyModel->setSourceModel(Model);

	ActionsProxyModel::ModelActionList beforeActions;
	beforeActions << ActionsProxyModel::ModelAction(tr(" - Select contact - "), "");
	ActionsModel = new ActionsProxyModel(beforeActions, ActionsProxyModel::ModelActionList(), this);
	ActionsModel->setSourceModel(ProxyModel);

	setModel(ActionsModel);
}

SelectBuddyCombobox::~SelectBuddyCombobox()
{
	delete Popup;
	Popup = 0;
}

void SelectBuddyCombobox::addFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->addFilter(filter);
	Popup->view()->addFilter(filter);
}

void SelectBuddyCombobox::removeFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->removeFilter(filter);
	Popup->view()->removeFilter(filter);
}

void SelectBuddyCombobox::showPopup()
{
	Popup->setGeometry(QRect(
			mapToGlobal(rect().bottomLeft()),
			QSize(geometry().width(), Popup->geometry().height())));
	Popup->show(buddy().display());
}

void SelectBuddyCombobox::hidePopup()
{
	Popup->hide();
}

Buddy SelectBuddyCombobox::buddy()
{
	return ProxyModel->buddyAt(view()->currentIndex());
}

void SelectBuddyCombobox::buddySelected(Buddy buddy)
{
	QModelIndex index = Model->buddyIndex(buddy);
	index = ProxyModel->mapFromSource(index);
	index = ActionsModel->mapFromSource(index);
	setCurrentIndex(index.row());
}
