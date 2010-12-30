/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtCore/QProcess>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

#include "configuration/configuration-file.h"
#include "buddies/buddy-manager.h"
#include "core/core.h"
#include "debug.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/buddies-list-widget.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "gui/hot-key.h"
#include "icons-manager.h"

#include "modules.h"
#include "misc/path-conversion.h"

#include "gui/windows/sms-image-dialog.h"
#include "gui/windows/sms-dialog.h"

#include "sms-actions.h"

#include "sms-configuration-ui-handler.h"

SmsConfigurationUiHandler *SmsConfigurationUiHandler::Instance = 0;

void SmsConfigurationUiHandler::registerConfigurationUi()
{
	if (!Instance)
	{
		Instance = new SmsConfigurationUiHandler();
		MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/sms.ui"));
		MainConfigurationWindow::registerUiHandler(smsConfigurationUiHandler);
	}
}

void SmsConfigurationUiHandler::unregisterConfigurationUi()
{
	if (Instance)
	{
		MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/sms.ui"));
		MainConfigurationWindow::unregisterUiHandler(smsConfigurationUiHandler);
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

void SmsConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	useBuiltIn = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widget()->widgetById("sms/useBuildInApp"));
	customApp = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("sms/customApp"));
	useCustomString = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widget()->widgetById("sms/useCustomString"));
	customString = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("sms/customString"));

	connect(useBuiltIn, SIGNAL(toggled(bool)), this, SLOT(onSmsBuildInCheckToggle(bool)));
	connect(useCustomString, SIGNAL(toggled(bool)), customString, SLOT(setEnabled(bool)));

}

void SmsConfigurationUiHandler::createDefaultConfiguration()
{
	config_file.addVariable("SMS", "Priority", QString());
	config_file.addVariable("SMS", "BuiltInApp", true);
	config_file.addVariable("SMS", "SmsNick", QString());
	config_file.addVariable("SMS", "UseCustomString", false);

	config_file.addVariable("ShortCuts", "kadu_sendsms", "Ctrl+S");
}

SmsConfigurationUiHandler *smsConfigurationUiHandler;
