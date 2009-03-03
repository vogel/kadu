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
#include <QtGui/QListWidget>

#include "gui/widgets/configuration/config-action-button.h"
#include "gui/widgets/configuration/config-group-box.h"

ConfigActionButton::ConfigActionButton(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QPushButton(parentConfigGroupBox->widget()), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox, dataManager)
{
	createWidgets();
}

ConfigActionButton::ConfigActionButton(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QPushButton(parentConfigGroupBox->widget()), ConfigWidget(parentConfigGroupBox, dataManager)
{
}

void ConfigActionButton::createWidgets()
{
	kdebugf();

	setText(qApp->tr("@default", widgetCaption.toAscii().data()));
	parentConfigGroupBox->addWidget(this);

	if (!ConfigWidget::toolTip.isEmpty())
		setToolTip(qApp->tr("@default", ConfigWidget::toolTip.toAscii().data()));
}

void ConfigActionButton::show()
{
	QPushButton::show();
}

void ConfigActionButton::hide()
{
	QPushButton::hide();
}
