/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef FIREWALL_CONFIGURATION_UI_HANDLER_H
#define FIREWALL_CONFIGURATION_UI_HANDLER_H

#include "gui/windows/main-configuration-window.h"

class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QTextEdit;

class MainConfigurationWindow;

class FirewallConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

	static FirewallConfigurationUiHandler *Instance;

	QListWidget *AllList;
	QListWidget *SecureList;
	QTextEdit *QuestionEdit;
	QLineEdit *AnswerEdit;

private slots:
	void left(QListWidgetItem *);
	void right(QListWidgetItem *);
	void allLeft();
	void allRight();

	void configurationApplied();

public:
	explicit FirewallConfigurationUiHandler();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	static void registerUiHandler();
	static void unregisterUiHandler();
};

#endif // FIREWALL_CONFIGURATION_UI_HANDLER_H
