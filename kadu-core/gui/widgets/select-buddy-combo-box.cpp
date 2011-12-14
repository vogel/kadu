/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "buddies/model/buddies-model.h"
#include "gui/widgets/select-buddy-popup.h"
#include "gui/widgets/talkable-tree-view.h"
#include "misc/misc.h"
#include "model/model-chain.h"
#include "model/roles.h"
#include "talkable/filter/hide-anonymous-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

#include "select-buddy-combo-box.h"

SelectBuddyComboBox::SelectBuddyComboBox(QWidget *parent) :
		ActionsComboBox(parent)
{
	addBeforeAction(new QAction(tr(" - Select buddy - "), this));

	ModelChain *chain = new ModelChain(new BuddiesModel(this), this);
	ProxyModel = new TalkableProxyModel(chain);
	ProxyModel->setSortByStatusAndUnreadMessages(false);
	chain->addProxyModel(ProxyModel);
	setUpModel(BuddyRole, chain);

	Popup = new SelectBuddyPopup();

	HideAnonymousTalkableFilter *hideAnonymousFilter = new HideAnonymousTalkableFilter(ProxyModel);
	addFilter(hideAnonymousFilter);

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

void SelectBuddyComboBox::addFilter(TalkableFilter *filter)
{
	ProxyModel->addFilter(filter);
	Popup->addFilter(filter);
}

void SelectBuddyComboBox::removeFilter(TalkableFilter *filter)
{
	ProxyModel->removeFilter(filter);
	Popup->removeFilter(filter);
}
