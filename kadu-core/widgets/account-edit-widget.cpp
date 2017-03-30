/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "widgets/composite-configuration-value-state-notifier.h"
#include "widgets/simple-configuration-value-state-notifier.h"

#include "account-configuration-widget-factory-repository.h"

#include "account-configuration-widget-factory.h"
#include "account-edit-widget.h"

AccountEditWidget::AccountEditWidget(Account account, QWidget *parent)
        : AccountConfigurationWidget(account, parent), StateNotifier(new SimpleConfigurationValueStateNotifier(this)),
          CompositeStateNotifier(new CompositeConfigurationValueStateNotifier(this))
{
    CompositeStateNotifier->addConfigurationValueStateNotifier(StateNotifier);
}

AccountEditWidget::~AccountEditWidget()
{
}

void AccountEditWidget::setAccountConfigurationWidgetFactoryRepository(
    AccountConfigurationWidgetFactoryRepository *accountConfigurationWidgetFactoryRepository)
{
    m_accountConfigurationWidgetFactoryRepository = accountConfigurationWidgetFactoryRepository;
}

void AccountEditWidget::init()
{
    connect(
        m_accountConfigurationWidgetFactoryRepository, SIGNAL(factoryRegistered(AccountConfigurationWidgetFactory *)),
        this, SLOT(factoryRegistered(AccountConfigurationWidgetFactory *)));
    connect(
        m_accountConfigurationWidgetFactoryRepository, SIGNAL(factoryUnregistered(AccountConfigurationWidgetFactory *)),
        this, SLOT(factoryUnregistered(AccountConfigurationWidgetFactory *)));

    for (auto factory : m_accountConfigurationWidgetFactoryRepository->factories())
        factoryRegistered(factory);
}

AccountConfigurationWidgetFactoryRepository *AccountEditWidget::accountConfigurationWidgetFactoryRepository() const
{
    return m_accountConfigurationWidgetFactoryRepository;
}

void AccountEditWidget::factoryRegistered(AccountConfigurationWidgetFactory *factory)
{
    AccountConfigurationWidget *widget = factory->createWidget(account(), this);
    if (widget)
    {
        if (widget->stateNotifier())
            CompositeStateNotifier->addConfigurationValueStateNotifier(widget->stateNotifier());
        AccountConfigurationWidgets.insert(factory, widget);
        emit widgetAdded(widget);
    }
}

void AccountEditWidget::factoryUnregistered(AccountConfigurationWidgetFactory *factory)
{
    if (AccountConfigurationWidgets.contains(factory))
    {
        AccountConfigurationWidget *widget = AccountConfigurationWidgets.value(factory);
        if (!widget)
            return;
        if (widget->stateNotifier())
            CompositeStateNotifier->removeConfigurationValueStateNotifier(widget->stateNotifier());
        AccountConfigurationWidgets.remove(factory);
        emit widgetRemoved(widget);
        widget->deleteLater();
    }
}

QList<AccountConfigurationWidget *> AccountEditWidget::accountConfigurationWidgets() const
{
    return AccountConfigurationWidgets.values();
}

void AccountEditWidget::applyAccountConfigurationWidgets()
{
    for (auto widget : AccountConfigurationWidgets)
        widget->apply();
}

void AccountEditWidget::cancelAccountConfigurationWidgets()
{
    for (auto widget : AccountConfigurationWidgets)
        widget->cancel();
}

SimpleConfigurationValueStateNotifier *AccountEditWidget::simpleStateNotifier() const
{
    return StateNotifier;
}

const ConfigurationValueStateNotifier *AccountEditWidget::stateNotifier() const
{
    return CompositeStateNotifier;
}

#include "moc_account-edit-widget.cpp"
