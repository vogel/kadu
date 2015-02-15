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

#ifndef COMPOSITE_CONFIGURATION_VALUE_STATE_NOTIFIER_H
#define COMPOSITE_CONFIGURATION_VALUE_STATE_NOTIFIER_H

#include "configuration-value-state-notifier.h"

class KADUAPI CompositeConfigurationValueStateNotifier : public ConfigurationValueStateNotifier
{
	Q_OBJECT

	QList<const ConfigurationValueStateNotifier *> StateNotifiers;
	ConfigurationValueState CurrentState;

	ConfigurationValueState computeState();

private slots:
	void recomputeState();

public:
	explicit CompositeConfigurationValueStateNotifier(QObject *parent = 0);
	virtual ~CompositeConfigurationValueStateNotifier();

	void addConfigurationValueStateNotifier(const ConfigurationValueStateNotifier *configurationValueStateNotifier);
	void removeConfigurationValueStateNotifier(const ConfigurationValueStateNotifier *configurationValueStateNotifier);

	virtual ConfigurationValueState state() const;

};

#endif // COMPOSITE_CONFIGURATION_VALUE_STATE_NOTIFIER_H
