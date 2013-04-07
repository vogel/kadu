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

#include "account-configuration-widget-repository.h"

#include "account-edit-widget.h"

AccountEditWidget::AccountEditWidget(AccountConfigurationWidgetRepository *configurationWidgetRepository, Account account, QWidget *parent) :
		AccountConfigurationWidget(account, parent), MyAccountConfigurationWidgetRepository(configurationWidgetRepository)
{
}

AccountEditWidget::~AccountEditWidget()
{
}

AccountConfigurationWidgetRepository * AccountEditWidget::accountConfigurationWidgetRepository() const
{
	return MyAccountConfigurationWidgetRepository;
}

void AccountEditWidget::createAccountConfigurationWidgets()
{
	AccountConfigurationWidgets = MyAccountConfigurationWidgetRepository->createWidgets(account(), this);
}

QList<AccountConfigurationWidget *> AccountEditWidget::accountConfigurationWidgets() const
{
	return AccountConfigurationWidgets;
}

void AccountEditWidget::applyAccountConfigurationWidgets()
{
	foreach (AccountConfigurationWidget *widget, AccountConfigurationWidgets)
		widget->apply();
}

void AccountEditWidget::cancelAccountConfigurationWidgets()
{
	foreach (AccountConfigurationWidget *widget, AccountConfigurationWidgets)
		widget->cancel();
}

ModalConfigurationWidgetState AccountEditWidget::accountConfigurationWidgetsState()
{
	bool anyChanged = false;

	foreach (AccountConfigurationWidget *widget, AccountConfigurationWidgets)
	{
		switch (widget->state())
		{
			case StateChangedDataInvalid:
				return StateChangedDataInvalid;
			case StateNotChanged:
				break;
			case StateChangedDataValid:
				anyChanged = true;
				break;
		}
	}

	if (anyChanged)
		return StateChangedDataValid;
	else
		return StateNotChanged;
}
