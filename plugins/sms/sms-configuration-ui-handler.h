/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef SMS_CONFIGURATION_UI_HANDLER_H
#define SMS_CONFIGURATION_UI_HANDLER_H

#include <QtCore/QBuffer>
#include <QtCore/QMap>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

#include "buddies/buddy-manager.h"
#include "chat/chat.h"
#include "gui/actions/action.h"
#include "gui/windows/main-configuration-window.h"

#include "sms_exports.h"

class ConfigComboBox;
class ConfigLineEdit;

class QCheckBox;
class QComboBox;
class QListWidget;
class QProcess;
class QTextEdit;

class SMSAPI SmsConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

	static SmsConfigurationUiHandler *Instance;

	QCheckBox *useBuiltIn;
	QLineEdit *customApp;
	QCheckBox *useCustomString;
	QLineEdit *customString;
	ConfigComboBox *EraGatewayComboBox;
	ConfigLineEdit *EraSponsoredUser;
	ConfigLineEdit *EraSponsoredPassword;
	ConfigLineEdit *EraOmnixUser;
	ConfigLineEdit *EraOmnixPassword;

	void createDefaultConfiguration();

	SmsConfigurationUiHandler();
	virtual ~SmsConfigurationUiHandler();

private slots:
	void onSmsBuildInCheckToggle(bool);
	void onEraGatewayChanged(int);

public:
	static void registerConfigurationUi();
	static void unregisterConfigurationUi();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
};

#endif // SMS_CONFIGURATION_UI_HANDLER_H
