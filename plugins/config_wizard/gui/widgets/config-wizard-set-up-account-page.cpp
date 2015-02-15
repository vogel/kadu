/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>

#include "accounts/account-manager.h"
#include "core/core.h"
#include "gui/widgets/account-add-widget.h"
#include "gui/widgets/account-create-widget.h"
#include "gui/windows/your-accounts.h"
#include "protocols/protocol-factory.h"

#include "config-wizard-set-up-account-page.h"

ConfigWizardSetUpAccountPage::ConfigWizardSetUpAccountPage(QWidget *parent) :
		ConfigWizardPage(parent), AccountSuccessfullyCreated(false)
{
	setDescription(tr("<p>Please enter your account data.</p><p>Go back if you want to select a different Account Setup option.</p>"));

	createGui();
}

ConfigWizardSetUpAccountPage::~ConfigWizardSetUpAccountPage()
{
}

void ConfigWizardSetUpAccountPage::createGui()
{
	formLayout()->addRow(new QLabel(tr("<h3>Account Setup</h3>"), this));
}

bool ConfigWizardSetUpAccountPage::isComplete() const
{
	if (AccountWidget && AccountWidget.data()->stateNotifier())
		return StateChangedDataValid == AccountWidget.data()->stateNotifier()->state();

	return true;
}

void ConfigWizardSetUpAccountPage::initializePage()
{
	ProtocolFactory *pf = field("choose-network.protocol-factory").value<ProtocolFactory *>();
	if (!pf)
		return;

	if (field("choose-network.new").toBool())
		AccountWidget = pf->newCreateAccountWidget(false, this);
	else if (field("choose-network.existing").toBool())
		AccountWidget = pf->newAddAccountWidget(false, this);

	if (AccountWidget)
	{
		formLayout()->addRow(QString(), AccountWidget.data());

		if (AccountWidget.data()->stateNotifier())
			connect(AccountWidget.data()->stateNotifier(), SIGNAL(stateChanged(ConfigurationValueState)), this, SIGNAL(completeChanged()));
		// NOTE: This signal is declared by AccountCreateWidget and AccountCreateWidget
		// but not by ModalConfigurationWidget. It will work correctly with Qt meta-object system, though.
		connect(AccountWidget.data(), SIGNAL(accountCreated(Account)), this, SLOT(accountCreated(Account)));
		// Same as above, window() is QWizard.
		connect(AccountWidget.data(), SIGNAL(destroyed()), window(), SLOT(back()));
	}
}

void ConfigWizardSetUpAccountPage::cleanupPage()
{
	if (AccountWidget)
	{
		disconnect(AccountWidget.data(), SIGNAL(destroyed()), window(), SLOT(back()));
		disconnect(AccountWidget.data(), 0, this, 0);
		delete AccountWidget.data();
	}

	QWizardPage::cleanupPage();
}

bool ConfigWizardSetUpAccountPage::validatePage()
{
	if (!AccountWidget)
		return true;

	AccountWidget.data()->apply();

	// apply() call should have blocked until accountCreated() was emitted,
	// so AccountSuccessfullyCreated should now be filled.
	return AccountSuccessfullyCreated;
}

void ConfigWizardSetUpAccountPage::accountCreated(Account account)
{
	if (!account)
	{
		AccountSuccessfullyCreated = false;
		return;
	}

	AccountManager::instance()->addItem(account);
	account.accountContact().setOwnerBuddy(Core::instance()->myself());

	AccountSuccessfullyCreated = true;

	ConfigurationManager::instance()->flush();
}

#include "moc_config-wizard-set-up-account-page.cpp"
