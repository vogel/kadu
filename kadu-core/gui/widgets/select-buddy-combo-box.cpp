/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QListView>

#include "buddies/buddy-manager.h"
#include "buddies/filter/anonymous-buddy-filter.h"
#include "buddies/filter/buddy-name-filter.h"
#include "buddies/model/buddies-model.h"
#include "buddies/model/buddies-model-proxy.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/select-buddy-popup.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"

#include "select-buddy-combo-box.h"

SelectBuddyComboBox::SelectBuddyComboBox(QWidget *parent) :
		QComboBox(parent)
{
	connect(this, SIGNAL(activated(int)), this, SLOT(activatedSlot()));

	Popup = new SelectBuddyPopup();
	Popup->view()->proxyModel()->setSortByStatus(false);
	connect(Popup, SIGNAL(buddySelected(Buddy)), this, SLOT(setBuddy(Buddy)));

	Model = new BuddiesModel(this);
	ProxyModel = new BuddiesModelProxy(this);
	ProxyModel->setSortByStatus(false);
	ProxyModel->setSourceModel(Model);

	ActionsModel = new ActionsProxyModel(this);
	ActionsModel->addBeforeAction(new QAction(tr(" - Select contact - "), this));
	ActionsModel->setSourceModel(ProxyModel);

	setModel(ActionsModel);

	AnonymousBuddyFilter *anonymousFilter = new AnonymousBuddyFilter(this);
	anonymousFilter->setEnabled(true);
	addFilter(anonymousFilter);
}

SelectBuddyComboBox::~SelectBuddyComboBox()
{
	delete Popup;
	Popup = 0;
}

void SelectBuddyComboBox::addFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->addFilter(filter);
	Popup->view()->addFilter(filter);
}

void SelectBuddyComboBox::removeFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->removeFilter(filter);
	Popup->view()->removeFilter(filter);
}

void SelectBuddyComboBox::showPopup()
{
	Popup->setGeometry(QRect(
			mapToGlobal(rect().bottomLeft()),
			QSize(geometry().width(), Popup->geometry().height())));
	Popup->show(buddy());
}

void SelectBuddyComboBox::hidePopup()
{
	Popup->hide();
}

Buddy SelectBuddyComboBox::buddy()
{
	QVariant buddyVariant = ActionsModel->index(currentIndex(), 0).data(BuddyRole);
	return buddyVariant.canConvert<Buddy>()
			? buddyVariant.value<Buddy>()
			: Buddy::null;
}

void SelectBuddyComboBox::activatedSlot()
{
	emit buddyChanged(buddy());
}

void SelectBuddyComboBox::setBuddy(Buddy buddy)
{
	QModelIndex index = Model->buddyIndex(buddy);
	index = ProxyModel->mapFromSource(index);
	index = ActionsModel->mapFromSource(index);
	setCurrentIndex(index.row());

	emit buddyChanged(buddy);
}
