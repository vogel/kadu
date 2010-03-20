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

#include "../history/history.h"

#include "gui/windows/sms-image-dialog.h"
#include "gui/windows/sms-dialog.h"

#include "sms-actions.h"
#include "sms.h"

extern "C" KADU_EXPORT int sms_init(bool firstLoad)
{
	SmsActions::registerActions(firstLoad);
	smsConfigurationUiHandler = new SmsConfigurationUiHandler();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/sms.ui"));
	MainConfigurationWindow::registerUiHandler(smsConfigurationUiHandler);
	return 0;
}

extern "C" KADU_EXPORT void sms_close()
{
  	SmsActions::unregisterActions();
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/sms.ui"));
	MainConfigurationWindow::unregisterUiHandler(smsConfigurationUiHandler);
	delete smsConfigurationUiHandler;
	smsConfigurationUiHandler = 0;
}

SmsConfigurationUiHandler::SmsConfigurationUiHandler()
	: gatewayListWidget(0)
{
	createDefaultConfiguration();
}

SmsConfigurationUiHandler::~SmsConfigurationUiHandler() 
{}


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

void SmsConfigurationUiHandler::configurationUpdated()
{
	if (!gatewayListWidget)
		return;

	QStringList priority;
	for (int index = 0; index < gatewayListWidget->count(); ++index)
			priority += gatewayListWidget->item(index)->text();

	// FIXME: : instead of ;
	config_file.writeEntry("SMS", "Priority", priority.join(";"));
}

void SmsConfigurationUiHandler::onUpButton()
{
	int index = gatewayListWidget->currentRow();
	if (index < 1)
		return;

	QListWidgetItem * item = gatewayListWidget->takeItem(index);
	gatewayListWidget->insertItem(--index, item);
	item->setSelected(true);
	gatewayListWidget->setCurrentItem(item);
}

void SmsConfigurationUiHandler::onDownButton()
{
	int index = gatewayListWidget->currentRow();
	if (index == gatewayListWidget->count() - 1 || index == -1)
		return;

	QListWidgetItem *item = gatewayListWidget->takeItem(index);
	gatewayListWidget->insertItem(++index, item);
	item->setSelected(true);
	gatewayListWidget->setCurrentItem(item);
}

void SmsConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(destroyed(QObject *)), this, SLOT(mainConfigurationWindowDestroyed()));

	useBuiltIn = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widget()->widgetById("sms/useBuildInApp"));
	customApp = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("sms/customApp"));
	useCustomString = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widget()->widgetById("sms/useCustomString"));
	customString = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("sms/customString"));

	ConfigGroupBox *gatewayGroupBox = mainConfigurationWindow->widget()->configGroupBox("SMS", "General", "Gateways");

	QWidget *gatewayWidget = new QWidget(gatewayGroupBox->widget());

	QHBoxLayout *gatewayLayout = new QHBoxLayout(gatewayWidget);
	gatewayLayout->setSpacing(5);

	gatewayListWidget = new QListWidget(gatewayWidget);

	QWidget *buttons = new QWidget(gatewayWidget);
	QVBoxLayout *buttonsLayout = new QVBoxLayout(buttons);
	buttonsLayout->setSpacing(5);

	QPushButton *up = new QPushButton(tr("Up"), buttons);
	QPushButton *down = new QPushButton(tr("Down"), buttons);

	buttonsLayout->addWidget(up);
	buttonsLayout->addWidget(down);
	buttonsLayout->addStretch(100);

	gatewayLayout->addWidget(gatewayListWidget);
	gatewayLayout->addWidget(buttons);

	connect(up, SIGNAL(clicked()), this, SLOT(onUpButton()));
	connect(down, SIGNAL(clicked()), this, SLOT(onDownButton()));

	gatewayGroupBox->addWidgets(new QLabel(tr("Gateways priorites"), gatewayGroupBox->widget()), gatewayWidget);
	
	connect(useBuiltIn, SIGNAL(toggled(bool)), this, SLOT(onSmsBuildInCheckToggle(bool)));
	connect(useCustomString, SIGNAL(toggled(bool)), customString, SLOT(setEnabled(bool)));

// TODO: fix it, should be ':' not ';'
	QStringList priority = config_file.readEntry("SMS", "Priority").split(";");

// 	foreach(const QString &gate, priority)
// 		if (SmsGatewayManager::instance()->gateways().contains(gate))
// 			gatewayListWidget->addItem(gate);

// 	foreach(const QString &key, SmsGatewayManager::instance()->gateways().keys())
// 		if (gatewayListWidget->findItems(key, 0).isEmpty())
// 			gatewayListWidget->addItem(key);
}

void SmsConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
	gatewayListWidget = 0; // protect configurationUpdated
}

void SmsConfigurationUiHandler::createDefaultConfiguration()
{
	config_file.addVariable("SMS", "Priority", QString::null);
	config_file.addVariable("SMS", "BuiltInApp", true);
	config_file.addVariable("SMS", "SmsNick", "");
	config_file.addVariable("SMS", "UseCustomString", false);

	config_file.addVariable("ShortCuts", "kadu_sendsms", "Ctrl+S");
}

SmsConfigurationUiHandler *smsConfigurationUiHandler;

