/*
 * %kadu copyright begin%
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "gui/widgets/configuration/config-proxy-combo-box.h"
#include "network/proxy/network-proxy-manager.h"

#include "debug.h"

ConfigProxyComboBox::ConfigProxyComboBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager) :
		ProxyComboBox(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), Label(0), ShowDefault(false)
{
}

ConfigProxyComboBox::~ConfigProxyComboBox()
{
	if (Label)
		delete Label;
}

void ConfigProxyComboBox::createWidgets()
{
	kdebugf();

	Label = new QLabel(QCoreApplication::translate("@default", widgetCaption.toUtf8().constData()) + ':', parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(Label, this);

	clear();

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(QCoreApplication::translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
		Label->setToolTip(QCoreApplication::translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
	}
}

void ConfigProxyComboBox::loadConfiguration()
{
	if (!dataManager)
		return;

	if (DefaultItem.isEmpty())
		setCurrentProxy(NetworkProxyManager::instance()->byUuid(dataManager->readEntry(section, item).toString()));

	QVariant defaultValue = dataManager->readEntry(section, DefaultItem);
	if (defaultValue.isNull() || defaultValue.toBool())
		selectDefaultProxy();
	else
		setCurrentProxy(NetworkProxyManager::instance()->byUuid(dataManager->readEntry(section, item).toString()));
}

void ConfigProxyComboBox::saveConfiguration()
{
	if (!dataManager)
		return;

	if (isDefaultProxySelected())
	{
		dataManager->writeEntry(section, item, "");
		dataManager->writeEntry(section, DefaultItem, true);
	}
	else
	{
		dataManager->writeEntry(section, item, currentProxy().uuid().toString());
		dataManager->writeEntry(section, DefaultItem, false);
	}
}

void ConfigProxyComboBox::setVisible(bool visible)
{
	Label->setVisible(visible);
	ProxyComboBox::setVisible(visible);
}

bool ConfigProxyComboBox::fromDomElement(QDomElement domElement)
{
	ShowDefault = QVariant(domElement.attribute("show-default", "false")).toBool();
	DefaultItem = domElement.attribute("config-item-default");

	if (ShowDefault)
		enableDefaultProxyAction();

	return ConfigWidgetValue::fromDomElement(domElement);
}

#include "moc_config-proxy-combo-box.cpp"
