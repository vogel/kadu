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

#include "gui/widgets/configuration/config-select-font.h"
#include "gui/widgets/configuration/config-group-box.h"

ConfigSelectFont::ConfigSelectFont(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: SelectFont(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigSelectFont::ConfigSelectFont(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: SelectFont(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
{
}

ConfigSelectFont::~ConfigSelectFont()
{
	if (label)
		delete label;
}

void ConfigSelectFont::createWidgets()
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

void ConfigSelectFont::loadConfiguration()
{
	if (section.isEmpty())
		return;
	QFont font;
	font.fromString(dataManager->readEntry(section, item).toString());
	setFont(font);
}

void ConfigSelectFont::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, QVariant(font().toString()));
}

void ConfigSelectFont::show()
{
	label->show();
	SelectFont::show();
}

void ConfigSelectFont::hide()
{
	label->hide();
	SelectFont::hide();
}