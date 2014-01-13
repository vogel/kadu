/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtXml/QDomElement>

#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-syntax-editor.h"

#include "debug.h"

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

	label = new QLabel(QCoreApplication::translate("@default", widgetCaption.toUtf8().constData()) + ':', parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(QCoreApplication::translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
		label->setToolTip(QCoreApplication::translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
	}
}

void ConfigSyntaxEditor::loadConfiguration()
{
	if (!dataManager)
		return;
	setCurrentSyntax(dataManager->readEntry(section, item).toString());
}

void ConfigSyntaxEditor::saveConfiguration()
{
	if (!dataManager)
		return;
	dataManager->writeEntry(section, item, currentSyntax());
}

void ConfigSyntaxEditor::setVisible(bool visible)
{
	label->setVisible(visible);
	SyntaxEditor::setVisible(visible);
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

#include "moc_config-syntax-editor.cpp"
