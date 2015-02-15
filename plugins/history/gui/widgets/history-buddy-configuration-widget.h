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

#ifndef HISTORY_BUDDY_CONFIGURATION_WIDGET_H
#define HISTORY_BUDDY_CONFIGURATION_WIDGET_H

#include "configuration/configuration-aware-object.h"
#include "gui/widgets/buddy-configuration-widget.h"

class QCheckBox;

class SimpleConfigurationValueStateNotifier;

class HistoryBuddyConfigurationWidget : public BuddyConfigurationWidget, ConfigurationAwareObject
{
	Q_OBJECT

	bool GlobalStoreHistory;
	QCheckBox *StoreHistoryCheckBox;

	SimpleConfigurationValueStateNotifier *StateNotifier;

	void createGui();
	void loadValues();

private slots:
	void updateState();

protected:
	virtual void configurationUpdated();

public:
	explicit HistoryBuddyConfigurationWidget(const Buddy &buddy, QWidget *parent = 0);
	virtual ~HistoryBuddyConfigurationWidget();

	virtual const ConfigurationValueStateNotifier * stateNotifier() const;

	virtual void apply();
	virtual void cancel();

};

#endif // HISTORY_BUDDY_CONFIGURATION_WIDGET_H
