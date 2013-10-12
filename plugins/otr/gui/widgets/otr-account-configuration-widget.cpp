/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "otr-policy-service.h"
#include "otr-policy.h"

#include "otr-account-configuration-widget.h"

OtrAccountConfigurationWidget::OtrAccountConfigurationWidget(const Account &account, QWidget *parent) :
		AccountConfigurationWidget(account, parent), StateNotifier(new SimpleConfigurationValueStateNotifier(this))
{
	setWindowTitle(tr("OTR Encryption"));

	createGui();
}

OtrAccountConfigurationWidget::~OtrAccountConfigurationWidget()
{
}

void OtrAccountConfigurationWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	EnableCheckBox = new QCheckBox(tr("Enable private messaging"));
	AutomaticallyInitiateCheckBox = new QCheckBox(tr("Automatically initiate private messaging"));
	RequireCheckBox = new QCheckBox(tr("Require private messaging"));

	connect(EnableCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateState()));
	connect(AutomaticallyInitiateCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateState()));
	connect(RequireCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateState()));

	layout->addWidget(EnableCheckBox);
	layout->addWidget(AutomaticallyInitiateCheckBox);
	layout->addWidget(RequireCheckBox);
	layout->addStretch(100);
}

void OtrAccountConfigurationWidget::setPolicyService(OtrPolicyService *policyService)
{
	PolicyService = policyService;

	loadValues();
	updateState();
}

OtrPolicy OtrAccountConfigurationWidget::policy()
{
	if (!EnableCheckBox->isChecked())
		return OtrPolicy::PolicyNever;
	if (!AutomaticallyInitiateCheckBox->isChecked())
		return OtrPolicy::PolicyManual;
	if (!RequireCheckBox->isChecked())
		return OtrPolicy::PolicyOpportunistic;
	return OtrPolicy::PolicyAlways;
}

void OtrAccountConfigurationWidget::loadValues()
{
	if (!PolicyService)
		return;

	OtrPolicy accountPolicy = PolicyService.data()->accountPolicy(account());

	if (accountPolicy == OtrPolicy::PolicyManual)
	{
		EnableCheckBox->setChecked(true);
		AutomaticallyInitiateCheckBox->setChecked(false);
		RequireCheckBox->setChecked(false);
	}
	else if (accountPolicy == OtrPolicy::PolicyOpportunistic)
	{
		EnableCheckBox->setChecked(true);
		AutomaticallyInitiateCheckBox->setChecked(true);
		RequireCheckBox->setChecked(false);
	}
	else if (accountPolicy == OtrPolicy::PolicyAlways)
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
	if (!PolicyService)
	{
		StateNotifier->setState(StateNotChanged);
		return;
	}

	AutomaticallyInitiateCheckBox->setEnabled(false);
	RequireCheckBox->setEnabled(false);

	if (EnableCheckBox->isChecked())
	{
		AutomaticallyInitiateCheckBox->setEnabled(true);
		if (AutomaticallyInitiateCheckBox->isChecked())
			RequireCheckBox->setEnabled(true);
	}

	OtrPolicy accountPolicy = PolicyService.data()->accountPolicy(account());
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
	if (PolicyService)
		PolicyService.data()->setAccountPolicy(account(), policy());
	updateState();
}

void OtrAccountConfigurationWidget::cancel()
{
	loadValues();
}
