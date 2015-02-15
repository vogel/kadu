/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ANTISTRING_CONFIGURATION_UI_HANDLER_H
#define ANTISTRING_CONFIGURATION_UI_HANDLER_H

#include "gui/windows/main-configuration-window.h"

class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QSpinBox;

class AntistringConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT
	Q_DISABLE_COPY(AntistringConfigurationUiHandler)

	static AntistringConfigurationUiHandler *Instance;

	QListWidget *ConditionListWidget;
	QLineEdit *ConditionWidget;
	QSpinBox *FactorWidget;

	AntistringConfigurationUiHandler();
	virtual ~AntistringConfigurationUiHandler();

	void updateConditionList();

private slots:
	void addCondition();
	void changeCondition();
	void deleteCondition();

	void applyConfiguration();

	void wordSelected(QListWidgetItem *item);

public:
	static void registerUiHandler();
	static void unregisterUiHandler();

	static AntistringConfigurationUiHandler * instance() { return Instance; }

public slots:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

#endif // ANTISTRING_CONFIGURATION_UI_HANDLER_H
