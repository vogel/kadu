/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtXml/QDomElement>

#include "configuration/configuration-window-data-manager.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-list-widget.h"

#include "debug.h"

ConfigListWidget::ConfigListWidget(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		const QStringList &itemValues, const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QListWidget(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	Q_UNUSED(itemValues)
	Q_UNUSED(itemCaptions)

	createWidgets();
}

ConfigListWidget::ConfigListWidget(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QListWidget(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
{
}

ConfigListWidget::~ConfigListWidget()
{
	if (label)
		delete label;
}

void ConfigListWidget::setItems(const QStringList &itemValues, const QStringList &itemCaptions)
{
	this->itemValues = itemValues;
	this->itemCaptions = itemCaptions;

	clear();
	addItems(itemCaptions);
}

void ConfigListWidget::setIcons(const QList<QIcon> &icons)
{
	const int c = qMin(count(), icons.count());
	for (int i = 0; i < c; i++)
	{
		QListWidgetItem *listItem = QListWidget::item(i);
		listItem->setIcon(icons.at(i));
	}
}

void ConfigListWidget::setCurrentItem(const QString &currentItem)
{
	setCurrentRow(itemValues.indexOf(currentItem));
}

void ConfigListWidget::createWidgets()
{
	kdebugf();

	label = new QLabel(QCoreApplication::translate("@default", widgetCaption.toUtf8().constData()) + ':', parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this, Qt::AlignRight | Qt::AlignTop);

	clear();
	addItems(itemCaptions);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(QCoreApplication::translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
		label->setToolTip(QCoreApplication::translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
	}
}

void ConfigListWidget::loadConfiguration()
{
	if (!dataManager)
		return;

	if (!section.isEmpty() && !ConfigWidgetValue::item.isEmpty())
		setCurrentItem(dataManager->readEntry(section, ConfigWidgetValue::item).toString());
}

void ConfigListWidget::saveConfiguration()
{
	if (!dataManager)
		return;

	if (!section.isEmpty() && !ConfigWidgetValue::item.isEmpty())
		dataManager->writeEntry(section, ConfigWidgetValue::item, currentItemValue());
}

void ConfigListWidget::setVisible(bool visible)
{
	label->setVisible(visible);
	QListWidget::setVisible(visible);
}

bool ConfigListWidget::fromDomElement(QDomElement domElement)
{
	QDomNodeList children = domElement.childNodes();
	uint length = children.length();
	for (uint i = 0; i < length; i++)
	{
		QDomNode node = children.item(static_cast<int>(i));
		if (node.isElement())
		{
			QDomElement element = node.toElement();
			if (element.tagName() != "item")
				continue;

			itemValues.append(element.attribute("value"));
			itemCaptions.append(element.attribute("caption"));

			addItem(QCoreApplication::translate("@default", element.attribute("caption").toUtf8().constData()));
		}
	}

	return ConfigWidgetValue::fromDomElement(domElement);
}

#include "moc_config-list-widget.cpp"
