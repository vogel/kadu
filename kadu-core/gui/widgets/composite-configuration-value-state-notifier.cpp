/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "composite-configuration-value-state-notifier.h"

CompositeConfigurationValueStateNotifier::CompositeConfigurationValueStateNotifier(QObject *parent) :
		ConfigurationValueStateNotifier(parent), CurrentState(StateNotChanged)
{
}

CompositeConfigurationValueStateNotifier::~CompositeConfigurationValueStateNotifier()
{
}

void CompositeConfigurationValueStateNotifier::addConfigurationValueStateNotifier(const ConfigurationValueStateNotifier *configurationValueStateNotifier)
{
	if (!configurationValueStateNotifier)
		return;
	if (StateNotifiers.contains(configurationValueStateNotifier))
		return;

	StateNotifiers.append(configurationValueStateNotifier);
	connect(configurationValueStateNotifier, SIGNAL(stateChanged(ConfigurationValueState)), this, SLOT(recomputeState()));
	recomputeState();
}

void CompositeConfigurationValueStateNotifier::removeConfigurationValueStateNotifier(const ConfigurationValueStateNotifier *configurationValueStateNotifier)
{
	if (!configurationValueStateNotifier)
		return;
	if (!StateNotifiers.contains(configurationValueStateNotifier))
		return;

	StateNotifiers.removeAll(configurationValueStateNotifier);
	disconnect(configurationValueStateNotifier, SIGNAL(stateChanged(ConfigurationValueState)), this, SLOT(recomputeState()));
	recomputeState();
}

ConfigurationValueState CompositeConfigurationValueStateNotifier::computeState()
{
	bool anyChanged = false;

	foreach (const ConfigurationValueStateNotifier * const stateNotifier, StateNotifiers)
	{
		switch (stateNotifier->state())
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

void CompositeConfigurationValueStateNotifier::recomputeState()
{
	ConfigurationValueState newState = computeState();
	if (CurrentState == newState)
		return;

	CurrentState = newState;
	emit stateChanged(CurrentState);
}

ConfigurationValueState CompositeConfigurationValueStateNotifier::state() const
{
	return CurrentState;
}
