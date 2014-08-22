/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

#include "gui/widgets/simple-configuration-value-state-notifier.h"

#include "otr-policy-service.h"
#include "otr-policy.h"

#include "otr-buddy-configuration-widget.h"

OtrBuddyConfigurationWidget::OtrBuddyConfigurationWidget(const Buddy &buddy, QWidget *parent) :
		BuddyConfigurationWidget(buddy, parent), StateNotifier(new SimpleConfigurationValueStateNotifier(this))
{
	setWindowTitle(tr("OTR Encryption"));

	createGui();
}

OtrBuddyConfigurationWidget::~OtrBuddyConfigurationWidget()
{
}

void OtrBuddyConfigurationWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	UseAccountPolicyCheckBox = new QCheckBox(tr("Use account's policy"));
	EnableCheckBox = new QCheckBox(tr("Enable private messaging"));
	AutomaticallyInitiateCheckBox = new QCheckBox(tr("Automatically initiate private messaging"));
	RequireCheckBox = new QCheckBox(tr("Require private messaging"));

	connect(UseAccountPolicyCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateState()));
	connect(EnableCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateState()));
	connect(AutomaticallyInitiateCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateState()));
	connect(RequireCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateState()));

	layout->addWidget(UseAccountPolicyCheckBox);
	layout->addWidget(EnableCheckBox);
	layout->addWidget(AutomaticallyInitiateCheckBox);
	layout->addWidget(RequireCheckBox);
	layout->addStretch(100);
}

void OtrBuddyConfigurationWidget::setPolicyService(OtrPolicyService *policyService)
{
	PolicyService = policyService;

	loadValues();
	updateState();
}

OtrPolicy OtrBuddyConfigurationWidget::policy()
{
	if (UseAccountPolicyCheckBox->isChecked())
		return OtrPolicy::PolicyUndefined;
	if (!EnableCheckBox->isChecked())
		return OtrPolicy::PolicyNever;
	if (!AutomaticallyInitiateCheckBox->isChecked())
		return OtrPolicy::PolicyManual;
	if (!RequireCheckBox->isChecked())
		return OtrPolicy::PolicyOpportunistic;
	return OtrPolicy::PolicyAlways;
}

void OtrBuddyConfigurationWidget::loadValues()
{
	if (!PolicyService)
		return;

	OtrPolicy buddyPolicy = PolicyService.data()->buddyPolicy(buddy());

	if (buddyPolicy == OtrPolicy::PolicyUndefined)
	{
		UseAccountPolicyCheckBox->setChecked(true);
		EnableCheckBox->setChecked(false);
		AutomaticallyInitiateCheckBox->setChecked(false);
		RequireCheckBox->setChecked(false);
	}
	else if (buddyPolicy == OtrPolicy::PolicyManual)
	{
		UseAccountPolicyCheckBox->setChecked(false);
		EnableCheckBox->setChecked(true);
		AutomaticallyInitiateCheckBox->setChecked(false);
		RequireCheckBox->setChecked(false);
	}
	else if (buddyPolicy == OtrPolicy::PolicyOpportunistic)
	{
		UseAccountPolicyCheckBox->setChecked(false);
		EnableCheckBox->setChecked(true);
		AutomaticallyInitiateCheckBox->setChecked(true);
		RequireCheckBox->setChecked(false);
	}
	else if (buddyPolicy == OtrPolicy::PolicyAlways)
	{
		UseAccountPolicyCheckBox->setChecked(false);
		EnableCheckBox->setChecked(true);
		AutomaticallyInitiateCheckBox->setChecked(true);
		RequireCheckBox->setChecked(true);
	}
	else
	{
		UseAccountPolicyCheckBox->setChecked(false);
		EnableCheckBox->setChecked(false);
		AutomaticallyInitiateCheckBox->setChecked(false);
		RequireCheckBox->setChecked(false);
	}
}

void OtrBuddyConfigurationWidget::updateState()
{
	if (!PolicyService)
	{
		StateNotifier->setState(StateNotChanged);
		return;
	}

	EnableCheckBox->setEnabled(false);
	AutomaticallyInitiateCheckBox->setEnabled(false);
	RequireCheckBox->setEnabled(false);

	if (!UseAccountPolicyCheckBox->isChecked())
	{
		EnableCheckBox->setEnabled(true);

		if (EnableCheckBox->isChecked())
		{
			AutomaticallyInitiateCheckBox->setEnabled(true);
			if (AutomaticallyInitiateCheckBox->isChecked())
				RequireCheckBox->setEnabled(true);
		}
	}

	OtrPolicy accountPolicy = PolicyService.data()->buddyPolicy(buddy());
	if (accountPolicy == policy())
		StateNotifier->setState(StateNotChanged);
	else
		StateNotifier->setState(StateChangedDataValid);
}

const ConfigurationValueStateNotifier * OtrBuddyConfigurationWidget::stateNotifier() const
{
	return StateNotifier;
}

void OtrBuddyConfigurationWidget::apply()
{
	if (PolicyService)
		PolicyService.data()->setBuddyPolicy(buddy(), policy());
	updateState();
}

void OtrBuddyConfigurationWidget::cancel()
{
	loadValues();
}
