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
#include <QtXml/QDomElement>

#include "gui/widgets/configuration/config-syntax-editor.h"
#include "gui/widgets/configuration/config-group-box.h"

ConfigSyntaxEditor::ConfigSyntaxEditor(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: SyntaxEditor(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigSyntaxEditor::ConfigSyntaxEditor(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: SyntaxEditor(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
{
}

ConfigSyntaxEditor::~ConfigSyntaxEditor()
{
	if (label)
		delete label;
}

void ConfigSyntaxEditor::createWidgets()
{
	kdebugf();

	label = new QLabel(qApp->tr("@default", widgetCaption.toAscii().data()) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->tr("@default", ConfigWidget::toolTip.toAscii().data()));
		label->setToolTip(qApp->tr("@default", ConfigWidget::toolTip.toAscii().data()));
	}
}

void ConfigSyntaxEditor::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setCurrentSyntax(dataManager->readEntry(section, item).toString());
}

void ConfigSyntaxEditor::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, currentSyntax());
}

void ConfigSyntaxEditor::show()
{
	label->show();
	SyntaxEditor::show();
}

void ConfigSyntaxEditor::hide()
{
	label->hide();
	SyntaxEditor::hide();
}

bool ConfigSyntaxEditor::fromDomElement(QDomElement domElement)
{
	QString category = domElement.attribute("category");
	QString syntaxHint = domElement.attribute("syntax-hint");
	if (category.isEmpty())
		return false;

	setCategory(category);
	setSyntaxHint(syntaxHint);

	return ConfigWidgetValue::fromDomElement(domElement);
}
