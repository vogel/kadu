/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2011 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
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
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

#include "buddies/buddy-manager.h"
#include "configuration/configuration-file.h"
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

#include "misc/kadu-paths.h"

#include "gui/windows/sms-dialog.h"

#include "sms-actions.h"

#include "sms-configuration-ui-handler.h"

SmsConfigurationUiHandler *SmsConfigurationUiHandler::Instance = 0;

void SmsConfigurationUiHandler::registerConfigurationUi()
{
	if (!Instance)
	{
		Instance = new SmsConfigurationUiHandler();
		MainConfigurationWindow::registerUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/sms.ui"));
		MainConfigurationWindow::registerUiHandler(Instance);
	}
}

void SmsConfigurationUiHandler::unregisterConfigurationUi()
{
	if (Instance)
	{
		MainConfigurationWindow::unregisterUiHandler(Instance);
		MainConfigurationWindow::unregisterUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/sms.ui"));
		delete Instance;
		Instance = 0;
	}
}

SmsConfigurationUiHandler::SmsConfigurationUiHandler()
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
	connect(useBuiltIn, SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("sms/proxy"), SLOT(setEnabled(bool)));
	connect(useCustomString, SIGNAL(toggled(bool)), customString, SLOT(setEnabled(bool)));

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
	config_file.addVariable("SMS", "Priority", QString());
	config_file.addVariable("SMS", "BuiltInApp", true);
	config_file.addVariable("SMS", "SmsNick", QString());
	config_file.addVariable("SMS", "UseCustomString", false);

	config_file.addVariable("ShortCuts", "kadu_sendsms", "Ctrl+S");
}
