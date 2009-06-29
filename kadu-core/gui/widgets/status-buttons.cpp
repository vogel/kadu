/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QHBoxLayout>

#include "accounts/account-manager.h"
#include "gui/widgets/status-button.h"

#include "status-buttons.h"

StatusButtons::StatusButtons(QWidget *parent) :
		QWidget(parent), Layout(0)
{
	createGui();

	triggerAllAccountsRegistered();
}

StatusButtons::~StatusButtons()
{
}

void StatusButtons::createGui()
{
	Layout = new QHBoxLayout(this);
}

void StatusButtons::accountRegistered(Account *account)
{
	if (Buttons.contains(account) || !Layout)
		return;

	StatusButton *button = new StatusButton(account);
	Layout->addWidget(button);
	Buttons[account] = button;
}

void StatusButtons::accountUnregistered(Account *account)
{
	if (Buttons.contains(account))
	{
		delete Buttons[account];
		Buttons.remove(account);
	}
}
