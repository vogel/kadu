/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QCheckBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>

#include "accounts/account-manager.h"
#include "gui/windows/your-accounts.h"

#include "config-wizard-completed-page.h"

ConfigWizardCompletedPage::ConfigWizardCompletedPage(QWidget *parent) :
		ConfigWizardPage(parent)
{
	setDescription("<p>Confirm your wizard setting to start using Kadu.</p>");

	createGui();
}

ConfigWizardCompletedPage::~ConfigWizardCompletedPage()
{
}

void ConfigWizardCompletedPage::createGui()
{
	formLayout()->addRow(new QLabel(tr("<h3>Configuration Wizard Completed</h3>"), this));

	ConfigureAccouuntsCheckBox = new QCheckBox(tr("Go to Accounts Setting after closing this window"), this);

	formLayout()->addRow("", ConfigureAccouuntsCheckBox);
}

void ConfigWizardCompletedPage::initializePage()
{
	ConfigureAccouuntsCheckBox->setChecked(AccountManager::instance()->items().isEmpty());
}

void ConfigWizardCompletedPage::acceptPage()
{
    if (ConfigureAccouuntsCheckBox->isChecked())
		YourAccounts::instance()->show();
}
