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

#include "gui/widgets/configuration/config-check-box.h"
#include "gui/widgets/configuration/config-group-box.h"

#include "debug.h"

ConfigCheckBox::ConfigCheckBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QCheckBox(widgetCaption, parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager)
{
	createWidgets();
}

ConfigCheckBox::ConfigCheckBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: QCheckBox(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager)
{
}

void ConfigCheckBox::createWidgets()
{
	kdebugf();

	setText(qApp->translate("@default", widgetCaption.toAscii().data()));
	parentConfigGroupBox->addWidget(this, true);

	if (!ConfigWidget::toolTip.isEmpty())
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toAscii().data()));
}

void ConfigCheckBox::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setChecked(dataManager->readEntry(section, item).toBool());
	emit toggled(isChecked());
}

void ConfigCheckBox::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, QVariant(isChecked() ? "true" : "false"));
}

void ConfigCheckBox::show()
{
	QCheckBox::show();
}

void ConfigCheckBox::hide()
{
	QCheckBox::hide();
}
