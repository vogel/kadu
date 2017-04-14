/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/injected-factory.h"
#include "misc/misc.h"
#include "model/model-chain.h"
#include "model/roles.h"
#include "talkable/filter/hide-anonymous-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"
#include "widgets/select-talkable-popup.h"

#include "select-talkable-combo-box.h"
#include "select-talkable-combo-box.moc"

SelectTalkableComboBox::SelectTalkableComboBox(QWidget *parent) : ActionsComboBox(parent)
{
}

SelectTalkableComboBox::~SelectTalkableComboBox()
{
}

void SelectTalkableComboBox::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void SelectTalkableComboBox::init()
{
    Chain = new ModelChain(this);
    ProxyModel = m_injectedFactory->makeInjected<TalkableProxyModel>(Chain);
    ProxyModel->setSortByStatusAndUnreadMessages(false);
    Chain->addProxyModel(ProxyModel);
    setUpModel(TalkableRole, Chain);

    Popup = m_injectedFactory->makeInjected<SelectTalkablePopup>(this);

    HideAnonymousFilter = new HideAnonymousTalkableFilter(ProxyModel);
    addFilter(HideAnonymousFilter);

    connect(Popup, SIGNAL(talkableSelected(Talkable)), this, SLOT(setCurrentTalkable(Talkable)));
}

InjectedFactory *SelectTalkableComboBox::injectedFactory() const
{
    return m_injectedFactory;
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
