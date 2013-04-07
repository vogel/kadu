/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/account-configuration-widget.h"
#include "gui/widgets/account-configuration-widget-factory.h"
#include "gui/widgets/account-configuration-widget-factory-repository.h"

#include "account-configuration-widget-repository.h"

AccountConfigurationWidgetRepository::AccountConfigurationWidgetRepository(QObject *parent) :
		QObject(parent)
{
}

AccountConfigurationWidgetRepository::~AccountConfigurationWidgetRepository()
{
}

void AccountConfigurationWidgetRepository::setFactoryRepository(AccountConfigurationWidgetFactoryRepository *factoryRepository)
{
	Q_ASSERT(!FactoryRepository);
	Q_ASSERT(factoryRepository);
	FactoryRepository = factoryRepository;

	connect(FactoryRepository.data(), SIGNAL(factoryUnregistered(AccountConfigurationWidgetFactory*)),
			this, SLOT(factoryUnregistered(AccountConfigurationWidgetFactory*)));
}

QList<AccountConfigurationWidget *> AccountConfigurationWidgetRepository::createWidgets(const Account &account, QWidget *parent)
{
	Q_ASSERT(FactoryRepository);
	QList<AccountConfigurationWidget *> result;

	foreach (AccountConfigurationWidgetFactory *factory, FactoryRepository.data()->factories())
	{
		AccountConfigurationWidget *widget = factory->createWidget(account, parent);
		if (widget)
		{
			addWidgetToMaps(factory, widget);
			result.append(widget);
		}
	}

	return result;
}

void AccountConfigurationWidgetRepository::addWidgetToMaps(AccountConfigurationWidgetFactory *factory, AccountConfigurationWidget *widget)
{
	if (!WidgetsByFactory.contains(factory))
		WidgetsByFactory.insert(factory, QList<AccountConfigurationWidget *>());
	WidgetsByFactory[factory].append(widget);
	FactoryByWidget.insert(widget, factory);
}

void AccountConfigurationWidgetRepository::factoryUnregistered(AccountConfigurationWidgetFactory *factory)
{
	if (!WidgetsByFactory.contains(factory))
		return;

	QList<AccountConfigurationWidget *> widgets = WidgetsByFactory.value(factory);
	foreach (AccountConfigurationWidget *widget, widgets)
	{
		FactoryByWidget.remove(widget);
		delete widget;
	}
}
