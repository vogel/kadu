/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QAction>

#include "gui/widgets/select-talkable-popup.h"
#include "misc/misc.h"
#include "model/model-chain.h"
#include "model/roles.h"
#include "talkable/filter/hide-anonymous-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

#include "select-talkable-combo-box.h"

SelectTalkableComboBox::SelectTalkableComboBox(QWidget *parent) :
		ActionsComboBox(parent)
{
	Chain = new ModelChain(this);
	ProxyModel = new TalkableProxyModel(Chain);
	ProxyModel->setSortByStatusAndUnreadMessages(false);
	Chain->addProxyModel(ProxyModel);
	setUpModel(TalkableRole, Chain);

	Popup = new SelectTalkablePopup(this);

	HideAnonymousFilter = new HideAnonymousTalkableFilter(ProxyModel);
	addFilter(HideAnonymousFilter);

	connect(Popup, SIGNAL(talkableSelected(Talkable)), this, SLOT(setCurrentTalkable(Talkable)));
}

SelectTalkableComboBox::~SelectTalkableComboBox()
{
}

void SelectTalkableComboBox::setBaseModel(QAbstractItemModel *model)
{
	Chain->setBaseModel(model);
	Popup->setBaseModel(model);

	setCurrentIndex(0);
}

void SelectTalkableComboBox::setShowAnonymous(bool showAnonymous)
{
	HideAnonymousFilter->setEnabled(!showAnonymous);
	Popup->setShowAnonymous(showAnonymous);
}

void SelectTalkableComboBox::setCurrentTalkable(const Talkable &talkable)
{
	setCurrentValue(QVariant::fromValue(talkable));
}

Talkable SelectTalkableComboBox::currentTalkable() const
{
	return currentValue().value<Talkable>();
}

void SelectTalkableComboBox::showPopup()
{
	QRect geom(mapToGlobal(rect().bottomLeft()), QSize(geometry().width(), Popup->sizeHint().height()));
	Popup->setGeometry(properGeometry(geom));

	Popup->show(currentTalkable());
}

void SelectTalkableComboBox::hidePopup()
{
	Popup->hide();
}

void SelectTalkableComboBox::addFilter(TalkableFilter *filter)
{
	ProxyModel->addFilter(filter);
	Popup->addFilter(filter);
}

void SelectTalkableComboBox::removeFilter(TalkableFilter *filter)
{
	ProxyModel->removeFilter(filter);
	Popup->removeFilter(filter);
}

#include "moc_select-talkable-combo-box.cpp"
