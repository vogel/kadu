/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QTabWidget>

#include "gui/widgets/account-edit-widget.h"

#include "account-configuration-widget-tab-adapter.h"

AccountConfigurationWidgetTabAdapter::AccountConfigurationWidgetTabAdapter(AccountEditWidget *accountEditWidget, QTabWidget *tabWidget, QObject *parent) :
		QObject(parent), MyAccountEditWidget(accountEditWidget), MyTabWidget(tabWidget)
{
	if (!MyAccountEditWidget || !MyTabWidget)
		return;

	connect(MyAccountEditWidget, SIGNAL(widgetAdded(AccountConfigurationWidget*)), this, SLOT(widgetAdded(AccountConfigurationWidget*)));

	foreach (AccountConfigurationWidget *widget, MyAccountEditWidget->accountConfigurationWidgets())
		widgetAdded(widget);
}

AccountConfigurationWidgetTabAdapter::~AccountConfigurationWidgetTabAdapter()
{
}

void AccountConfigurationWidgetTabAdapter::widgetAdded(AccountConfigurationWidget *widget)
{
	MyTabWidget->addTab(widget, widget->windowTitle());
}

#include "moc_account-configuration-widget-tab-adapter.cpp"
