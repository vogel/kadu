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

#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-group-box.h"

ConfigComboBox::ConfigComboBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		const QStringList &itemValues, const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QComboBox(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigComboBox::ConfigComboBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QComboBox(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
{
}

ConfigComboBox::~ConfigComboBox()
{
	if (label)
		delete label;
}

void ConfigComboBox::setItems(const QStringList &itemValues, const QStringList &itemCaptions)
{
	this->itemValues = itemValues;
	this->itemCaptions = itemCaptions;

	clear();
	insertItems(0, itemCaptions);
}

QString ConfigComboBox::currentItemValue()
{
	int index = currentIndex();

	if ((index < 0) || (index >= itemValues.size()))
		return QString::null;

	return itemValues[index];
}

void ConfigComboBox::createWidgets()
{
	kdebugf();

	label = new QLabel(qApp->tr("@default", widgetCaption.toAscii().data()) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	clear();
	insertItems(0, itemCaptions);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->tr("@default", ConfigWidget::toolTip.toAscii().data()));
		label->setToolTip(qApp->tr("@default", ConfigWidget::toolTip.toAscii().data()));
	}
}

void ConfigComboBox::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setCurrentIndex(itemValues.indexOf(dataManager->readEntry(section, item).toString()));

	emit activated(currentIndex());
}

void ConfigComboBox::saveConfiguration()
{
	if (section.isEmpty())
		return;

	int index = currentIndex();

	if ((index < 0) || (index >= itemValues.size()))
		return;

	dataManager->writeEntry(section, item, QVariant(itemValues[currentIndex()]));
}

void ConfigComboBox::show()
{
	label->show();
	QComboBox::show();
}

void ConfigComboBox::hide()
{
	label->hide();
	QComboBox::hide();
}

bool ConfigComboBox::fromDomElement(QDomElement domElement)
{
	QDomNodeList children = domElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
	{
		QDomNode node = children.item(i);
		if (node.isElement())
		{
			QDomElement element = node.toElement();
			if (element.tagName() != "item")
				continue;

			itemValues.append(element.attribute("value"));
			itemCaptions.append(qApp->tr("@default", element.attribute("caption").toAscii().data()));

			addItem(qApp->tr("@default", element.attribute("caption").toAscii().data()));
		}
	}

	return ConfigWidgetValue::fromDomElement(domElement);
}
