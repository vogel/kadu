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
#include "model/roles.h"

#include "select-buddy-combo-box.h"

SelectBuddyComboBox::SelectBuddyComboBox(QWidget *parent) :
		KaduComboBox<Buddy>(parent)
{
	setUpModel(new BuddiesModel(this), new BuddiesModelProxy(this));

	Popup = new SelectBuddyPopup();
	Popup->view()->proxyModel()->setSortByStatus(false);

	static_cast<BuddiesModelProxy *>(SourceProxyModel)->setSortByStatus(false);

	AnonymousBuddyFilter *anonymousFilter = new AnonymousBuddyFilter(this);
	addFilter(anonymousFilter);

	connect(Popup, SIGNAL(buddySelected(Buddy)), this, SLOT(setCurrentBuddy(Buddy)));
	connect(model(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
			this, SLOT(updateValueBeforeChange()));
	connect(model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
			this, SLOT(rowsRemoved(const QModelIndex &, int, int)));
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));
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
	return currentValue();
}

void SelectBuddyComboBox::currentIndexChangedSlot(int index)
{
	if (KaduComboBox<Buddy>::currentIndexChangedSlot(index))
		emit buddyChanged(CurrentValue);
}

void SelectBuddyComboBox::updateValueBeforeChange()
{
	KaduComboBox<Buddy>::updateValueBeforeChange();
}

void SelectBuddyComboBox::rowsRemoved(const QModelIndex &parent, int start, int end)
{
	KaduComboBox<Buddy>::rowsRemoved(parent, start, end);
}

int SelectBuddyComboBox::preferredDataRole() const
{
	return BuddyRole;
}

QString SelectBuddyComboBox::selectString() const
{
	return tr(" - Select buddy - ");
}

void SelectBuddyComboBox::showPopup()
{
	Popup->setGeometry(QRect(
			mapToGlobal(rect().bottomLeft()),
			QSize(geometry().width(), Popup->geometry().height())));
	Popup->show(CurrentValue);
}

void SelectBuddyComboBox::hidePopup()
{
	Popup->hide();
}

void SelectBuddyComboBox::addFilter(AbstractBuddyFilter *filter)
{
	static_cast<BuddiesModelProxy *>(SourceProxyModel)->addFilter(filter);
	Popup->view()->addFilter(filter);
}

void SelectBuddyComboBox::removeFilter(AbstractBuddyFilter *filter)
{
	static_cast<BuddiesModelProxy *>(SourceProxyModel)->removeFilter(filter);
	Popup->view()->removeFilter(filter);
}
