/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>

#include "accounts/account-manager.h"
#include "gui/widgets/protocols-combo-box.h"
#include "gui/windows/your-accounts.h"

#include "config-wizard-choose-network-page.h"

ConfigWizardChooseNetworkPage::ConfigWizardChooseNetworkPage(QWidget *parent) :
		ConfigWizardPage(parent), LastProtocol(0)
{
	setDescription(tr("<p>Please choose the network for the account that you would like to set up.</p><p>You can also create a new account in the wizard if you don't already have one</p>"));

	createGui();
}

ConfigWizardChooseNetworkPage::~ConfigWizardChooseNetworkPage()
{
}

void ConfigWizardChooseNetworkPage::createGui()
{
	formLayout()->addRow(new QLabel(tr("<h3>Account Setup</h3>"), this));

	SelectProtocol = new ProtocolsComboBox(this);
	connect(SelectProtocol, SIGNAL(currentIndexChanged(int)),
			this, SLOT(protocolChanged()));

	formLayout()->addRow(tr("IM Network"), SelectProtocol);

	SetUpExisting = new QRadioButton(tr("I want to set up existing account for Kadu"), this);
	SetUpNew = new QRadioButton(tr("I want to create new account for Kadu"), this);
	Ignore = new QRadioButton(tr("I don't want to set up my account for Kadu now"), this);

	formLayout()->addRow(QString(), SetUpExisting);
	formLayout()->addRow(QString(), SetUpNew);
	formLayout()->addRow(QString(), Ignore);

	registerField("choose-network.protocol-factory", SelectProtocol, "currentProtocol", SIGNAL(currentIndexChanged(int)));
	registerField("choose-network.existing", SetUpExisting);
	registerField("choose-network.new", SetUpNew);
	registerField("choose-network.ignore", Ignore);

	protocolChanged();
}

void ConfigWizardChooseNetworkPage::protocolChanged()
{
	ProtocolFactory *protocol = SelectProtocol->currentProtocol();
	if (!protocol)
	{
		SetUpExisting->setEnabled(false);
		SetUpNew->setEnabled(false);
		Ignore->setChecked(true);
	}
	else
	{
		SetUpExisting->setEnabled(true);
		SetUpNew->setEnabled(protocol->canRegister());

		if (!LastProtocol || (SetUpNew->isChecked() && !SetUpNew->isEnabled()))
			SetUpExisting->setChecked(true);
	}

	LastProtocol = protocol;
}

#include "moc_config-wizard-choose-network-page.cpp"
