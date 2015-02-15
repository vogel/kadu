/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QProcess>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

#include "buddies/buddy-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/hot-key.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-line-edit.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "debug.h"

#include "misc/paths-provider.h"

#include "gui/windows/sms-dialog.h"

#include "sms-actions.h"

#include "sms-configuration-ui-handler.h"

SmsConfigurationUiHandler *SmsConfigurationUiHandler::Instance = 0;

void SmsConfigurationUiHandler::registerConfigurationUi()
{
	if (!Instance)
	{
		Instance = new SmsConfigurationUiHandler();
		MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/sms.ui"));
		MainConfigurationWindow::registerUiHandler(Instance);
	}
}

void SmsConfigurationUiHandler::unregisterConfigurationUi()
{
	if (Instance)
	{
		MainConfigurationWindow::unregisterUiHandler(Instance);
		MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/sms.ui"));
		delete Instance;
		Instance = 0;
	}
}

SmsConfigurationUiHandler::SmsConfigurationUiHandler() :
	useBuiltIn{},
	customApp{},
	useCustomString{},
	customString{},
	EraGatewayComboBox{},
	EraSponsoredUser{},
	EraSponsoredPassword{},
	EraOmnixUser{},
	EraOmnixPassword{}
{
	createDefaultConfiguration();
}

SmsConfigurationUiHandler::~SmsConfigurationUiHandler()
{
}

void SmsConfigurationUiHandler::onSmsBuildInCheckToggle(bool value)
{
	if (value)
	{
		customApp->setEnabled(false);
		useCustomString->setEnabled(false);
		customString->setEnabled(false);
	}
	else
	{
		customApp->setEnabled(true);
		useCustomString->setEnabled(true);
		customString->setEnabled(useCustomString->isChecked());
	}
}

void SmsConfigurationUiHandler::onEraGatewayChanged(int index)
{
	Q_UNUSED(index)

	QString gateway = EraGatewayComboBox->currentItemValue();

	if (gateway == "Sponsored")
	{
	    EraSponsoredUser->show();
	    EraSponsoredPassword->show();
	    EraOmnixUser->hide();
	    EraOmnixPassword->hide();
	}
	else
	{
	    EraSponsoredUser->hide();
	    EraSponsoredPassword->hide();
	    EraOmnixUser->show();
	    EraOmnixPassword->show();
	}
}

void SmsConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	useBuiltIn = static_cast<QCheckBox *>(mainConfigurationWindow->widget()->widgetById("sms/useBuildInApp"));
	customApp = static_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("sms/customApp"));
	useCustomString = static_cast<QCheckBox *>(mainConfigurationWindow->widget()->widgetById("sms/useCustomString"));
	customString = static_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("sms/customString"));

	connect(useBuiltIn, SIGNAL(toggled(bool)), this, SLOT(onSmsBuildInCheckToggle(bool)));

	EraGatewayComboBox = static_cast<ConfigComboBox *>(mainConfigurationWindow->widget()->widgetById("default_sms/eraGateway"));
	connect(EraGatewayComboBox, SIGNAL(activated(int)), this, SLOT(onEraGatewayChanged(int)));

	EraSponsoredUser = static_cast<ConfigLineEdit *>(mainConfigurationWindow->widget()->widgetById("default_sms/sponsoredUser"));
	EraSponsoredPassword = static_cast<ConfigLineEdit *>(mainConfigurationWindow->widget()->widgetById("default_sms/sponsoredPassword"));
	EraOmnixUser = static_cast<ConfigLineEdit *>(mainConfigurationWindow->widget()->widgetById("default_sms/multimediaUser"));
	EraOmnixPassword = static_cast<ConfigLineEdit *>(mainConfigurationWindow->widget()->widgetById("default_sms/multimediaPassword"));

	EraSponsoredPassword->setEchoMode(QLineEdit::Password);
	EraOmnixPassword->setEchoMode(QLineEdit::Password);
}

void SmsConfigurationUiHandler::createDefaultConfiguration()
{
	Application::instance()->configuration()->deprecatedApi()->addVariable("SMS", "Priority", QString());
	Application::instance()->configuration()->deprecatedApi()->addVariable("SMS", "BuiltInApp", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("SMS", "SmsNick", QString());
	Application::instance()->configuration()->deprecatedApi()->addVariable("SMS", "UseCustomString", false);

	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_sendsms", "Ctrl+S");
}

#include "moc_sms-configuration-ui-handler.cpp"
