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

#include "gui/widgets/configuration/config-color-button.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-widget-value.h"

ConfigColorButton::ConfigColorButton(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: ColorButton(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigColorButton::ConfigColorButton(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: ColorButton(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
{
}

ConfigColorButton::~ConfigColorButton()
{
	if (label)
		delete label;
}

void ConfigColorButton::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigColorButton::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setColor(dataManager->readEntry(section, item).value<QColor>());
}

void ConfigColorButton::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, QVariant(color().name()));
}

void ConfigColorButton::show()
{
	label->show();
	ColorButton::show();
}

void ConfigColorButton::hide()
{
	label->hide();
	ColorButton::hide();
}