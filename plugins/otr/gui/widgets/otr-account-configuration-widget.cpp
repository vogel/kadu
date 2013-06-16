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

#include "gui/widgets/simple-configuration-value-state-notifier.h"

#include "otr-policy.h"
#include "otr-policy-account-store.h"

#include "otr-account-configuration-widget.h"

OtrAccountConfigurationWidget::OtrAccountConfigurationWidget(const Account &account, QWidget *parent) :
		AccountConfigurationWidget(account, parent), StateNotifier(new SimpleConfigurationValueStateNotifier(this))
{
	setWindowTitle(tr("OTR Encyption"));

	createGui();
}

OtrAccountConfigurationWidget::~OtrAccountConfigurationWidget()
{
}

void OtrAccountConfigurationWidget::createGui()
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

OtrPolicy OtrAccountConfigurationWidget::policy()
{
	if (!EnableCheckBox->isChecked())
		return OtrPolicy::NEVER;
	if (!AutomaticallyInitiateCheckBox->isChecked())
		return OtrPolicy::MANUAL;
	if (!RequireCheckBox->isChecked())
		return OtrPolicy::OPPORTUNISTIC;
	return OtrPolicy::ALWAYS;
}

void OtrAccountConfigurationWidget::loadValues()
{
	OtrPolicy accountPolicy = OtrPolicyAccountStore::loadPolicyFromAccount(account());

	if (accountPolicy == OtrPolicy::MANUAL)
	{
		EnableCheckBox->setChecked(true);
		AutomaticallyInitiateCheckBox->setChecked(false);
		RequireCheckBox->setChecked(false);
	}
	else if (accountPolicy == OtrPolicy::OPPORTUNISTIC)
	{
		EnableCheckBox->setChecked(true);
		AutomaticallyInitiateCheckBox->setChecked(true);
		RequireCheckBox->setChecked(false);
	}
	else if (accountPolicy == OtrPolicy::MANUAL)
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

void OtrAccountConfigurationWidget::updateState()
{
	AutomaticallyInitiateCheckBox->setEnabled(false);
	RequireCheckBox->setEnabled(false);

	if (EnableCheckBox->isChecked())
	{
		AutomaticallyInitiateCheckBox->setEnabled(true);
		if (AutomaticallyInitiateCheckBox->isChecked())
			RequireCheckBox->setEnabled(true);
	}

	OtrPolicy accountPolicy = OtrPolicyAccountStore::loadPolicyFromAccount(account());
	if (accountPolicy == policy())
		StateNotifier->setState(StateNotChanged);
	else
		StateNotifier->setState(StateChangedDataValid);
}

const ConfigurationValueStateNotifier * OtrAccountConfigurationWidget::stateNotifier() const
{
	return StateNotifier;
}

void OtrAccountConfigurationWidget::apply()
{
	OtrPolicyAccountStore::storePolicyToAccount(account(), policy());
}

void OtrAccountConfigurationWidget::cancel()
{
	loadValues();
}
