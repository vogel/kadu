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
#include "buddies/filter/anonymous-buddy-filter.h"
#include "buddies/filter/buddy-name-filter.h"
#include "buddies/model/buddies-model.h"
#include "buddies/model/buddies-model-proxy.h"
#include "gui/widgets/buddies-line-edit.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/select-buddy-popup.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"

#include "select-buddy-combobox.h"

SelectBuddyCombobox::SelectBuddyCombobox(QWidget *parent) :
		QComboBox(parent)
{
	connect(this, SIGNAL(activated(int)), this, SLOT(activatedSlot()));

	Popup = new SelectBuddyPopup();
	Popup->view()->proxyModel()->setSortByStatus(false);
	connect(Popup, SIGNAL(buddySelected(Buddy)), this, SLOT(buddySelected(Buddy)));

	Model = new BuddiesModel(BuddyManager::instance(), this);
	ProxyModel = new BuddiesModelProxy(this);
	ProxyModel->setSortByStatus(false);
	ProxyModel->setSourceModel(Model);

	ActionsProxyModel::ModelActionList beforeActions;
	beforeActions << ActionsProxyModel::ModelAction(tr(" - Select contact - "), "");
	ActionsModel = new ActionsProxyModel(beforeActions, ActionsProxyModel::ModelActionList(), this);
	ActionsModel->setSourceModel(ProxyModel);

	setModel(ActionsModel);

	AnonymousBuddyFilter *anonymousFilter = new AnonymousBuddyFilter(this);
	anonymousFilter->setEnabled(true);
	addFilter(anonymousFilter);
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
	Popup->show(buddy());
}

void SelectBuddyCombobox::hidePopup()
{
	Popup->hide();
}

Buddy SelectBuddyCombobox::buddy()
{
	QVariant buddyVariant = ActionsModel->index(currentIndex(), 0).data(BuddyRole);
	return buddyVariant.canConvert<Buddy>()
			? buddyVariant.value<Buddy>()
			: Buddy::null;
}

void SelectBuddyCombobox::activatedSlot()
{
	emit buddyChanged(buddy());
}

void SelectBuddyCombobox::buddySelected(Buddy buddy)
{
	QModelIndex index = Model->buddyIndex(buddy);
	index = ProxyModel->mapFromSource(index);
	index = ActionsModel->mapFromSource(index);
	setCurrentIndex(index.row());

	emit buddyChanged(buddy);
}
