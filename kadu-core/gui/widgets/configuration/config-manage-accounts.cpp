/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QLabel>

#include "gui/widgets/configuration/config-manage-accounts.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "configuration/configuration-window-data-manager.h"

ConfigManageAccounts::ConfigManageAccounts(const QString &widgetCaption,
		const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name)
	: ManageAccounts(parentConfigGroupBox->widget()), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox, dataManager)
{
}

ConfigManageAccounts::ConfigManageAccounts(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: ManageAccounts(parentConfigGroupBox->widget()), ConfigWidget(parentConfigGroupBox, dataManager)
{
}

ConfigManageAccounts::~ConfigManageAccounts()
{
}

void ConfigManageAccounts::createWidgets()
{
	if (!ConfigWidget::toolTip.isEmpty())
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toAscii().data()));

	parentConfigGroupBox->addWidget(this, true);
}

void ConfigManageAccounts::loadConfiguration()
{
}

void ConfigManageAccounts::saveConfiguration()
{
}

void ConfigManageAccounts::show()
{
	ManageAccounts::show();
}

void ConfigManageAccounts::hide()
{
	ManageAccounts::hide();
}
