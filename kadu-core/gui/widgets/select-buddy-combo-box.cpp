/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/filter/anonymous-buddy-filter.h"
#include "buddies/filter/buddy-name-filter.h"
#include "buddies/model/buddies-model.h"
#include "buddies/model/buddies-model-proxy.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/select-buddy-popup.h"
#include "misc/misc.h"
#include "model/roles.h"

#include "select-buddy-combo-box.h"

SelectBuddyComboBox::SelectBuddyComboBox(QWidget *parent) :
		ActionsComboBox(parent)
{
	setDataRole(BuddyRole);

	ProxyModel = new BuddiesModelProxy(this);
	setUpModel(new BuddiesModel(this), ProxyModel);
	addBeforeAction(new QAction(tr(" - Select buddy - "), this));

	Popup = new SelectBuddyPopup();
	Popup->view()->proxyModel()->setSortByStatus(false);

	ProxyModel->setSortByStatus(false);

	AnonymousBuddyFilter *anonymousFilter = new AnonymousBuddyFilter(this);
	anonymousFilter->setEnabled(true);
	addFilter(anonymousFilter);

	connect(Popup, SIGNAL(buddySelected(Buddy)), this, SLOT(setCurrentBuddy(Buddy)));
}

SelectBuddyComboBox::~SelectBuddyComboBox()
{
	delete Popup;
	Popup = 0;
}

void SelectBuddyComboBox::setCurrentBuddy(Buddy buddy)
{
	setCurrentValue(buddy);
}

Buddy SelectBuddyComboBox::currentBuddy()
{
	return currentValue().value<Buddy>();
}

void SelectBuddyComboBox::valueChanged(QVariant value, QVariant previousValue)
{
	Q_UNUSED(previousValue)

	emit buddyChanged(value.value<Buddy>());
}

bool SelectBuddyComboBox::compare(QVariant value, QVariant previousValue) const
{
	return value.value<Buddy>() == previousValue.value<Buddy>();
}

void SelectBuddyComboBox::showPopup()
{
	QRect geom(mapToGlobal(rect().bottomLeft()), QSize(geometry().width(), Popup->height()));
	setWindowGeometry(Popup, geom);

	Popup->show(currentBuddy());
}

void SelectBuddyComboBox::hidePopup()
{
	Popup->hide();
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
