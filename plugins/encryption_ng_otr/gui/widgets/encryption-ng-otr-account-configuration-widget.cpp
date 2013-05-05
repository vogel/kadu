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

#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

#include "encryption-ng-otr-policy.h"
#include "encryption-ng-otr-policy-account-store.h"

#include "encryption-ng-otr-account-configuration-widget.h"

EncryptionNgOtrAccountConfigurationWidget::EncryptionNgOtrAccountConfigurationWidget(const Account &account, QWidget *parent) :
		AccountConfigurationWidget(account, parent)
{
	setWindowTitle(tr("OTR Encyption"));

	createGui();
}

EncryptionNgOtrAccountConfigurationWidget::~EncryptionNgOtrAccountConfigurationWidget()
{
}

void EncryptionNgOtrAccountConfigurationWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	EnableCheckBox = new QCheckBox(tr("Enable private nessaging"));
	AutomaticallyInitiateCheckBox = new QCheckBox(tr("Automatically initiate private messaging"));
	RequireCheckBox = new QCheckBox(tr("Require private messaging"));

	connect(EnableCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateState()));
	connect(AutomaticallyInitiateCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateState()));
	connect(RequireCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateState()));

	layout->addWidget(EnableCheckBox);
	layout->addWidget(AutomaticallyInitiateCheckBox);
	layout->addWidget(RequireCheckBox);
	layout->addStretch(100);

	loadValues();
	updateState();
}

EncryptionNgOtrPolicy EncryptionNgOtrAccountConfigurationWidget::policy()
{
	if (!EnableCheckBox->isChecked())
		return EncryptionNgOtrPolicy::NEVER;
	if (!AutomaticallyInitiateCheckBox->isChecked())
		return EncryptionNgOtrPolicy::MANUAL;
	if (!RequireCheckBox->isChecked())
		return EncryptionNgOtrPolicy::OPPORTUNISTIC;
	return EncryptionNgOtrPolicy::ALWAYS;
}

void EncryptionNgOtrAccountConfigurationWidget::loadValues()
{
	EncryptionNgOtrPolicy accountPolicy = EncryptionNgOtrPolicyAccountStore::loadPolicyFromAccount(account());

	if (accountPolicy == EncryptionNgOtrPolicy::MANUAL)
	{
		EnableCheckBox->setChecked(true);
		AutomaticallyInitiateCheckBox->setChecked(false);
		RequireCheckBox->setChecked(false);
	}
	else if (accountPolicy == EncryptionNgOtrPolicy::OPPORTUNISTIC)
	{
		EnableCheckBox->setChecked(true);
		AutomaticallyInitiateCheckBox->setChecked(true);
		RequireCheckBox->setChecked(false);
	}
	else if (accountPolicy == EncryptionNgOtrPolicy::MANUAL)
	{
		EnableCheckBox->setChecked(true);
		AutomaticallyInitiateCheckBox->setChecked(true);
		RequireCheckBox->setChecked(true);
	}
	else
	{
		EnableCheckBox->setChecked(false);
		AutomaticallyInitiateCheckBox->setChecked(false);
		RequireCheckBox->setChecked(false);
	}
}

void EncryptionNgOtrAccountConfigurationWidget::updateState()
{
	AutomaticallyInitiateCheckBox->setEnabled(false);
	RequireCheckBox->setEnabled(false);

	if (EnableCheckBox->isChecked())
	{
		AutomaticallyInitiateCheckBox->setEnabled(true);
		if (AutomaticallyInitiateCheckBox->isChecked())
			RequireCheckBox->setEnabled(true);
	}

	EncryptionNgOtrPolicy accountPolicy = EncryptionNgOtrPolicyAccountStore::loadPolicyFromAccount(account());
	if (accountPolicy == policy())
		setState(StateNotChanged);
	else
		setState(StateChangedDataValid);
}

void EncryptionNgOtrAccountConfigurationWidget::apply()
{
	EncryptionNgOtrPolicyAccountStore::storePolicyToAccount(account(), policy());
}

void EncryptionNgOtrAccountConfigurationWidget::cancel()
{
	loadValues();
}
