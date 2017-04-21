/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "config-proxy-combo-box.h"
#include "config-proxy-combo-box.moc"

#include "network/proxy/network-proxy-manager.h"
#include "widgets/configuration/config-group-box.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtXml/QDomElement>

ConfigProxyComboBox::ConfigProxyComboBox(
    const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
    const QStringList &itemValues, const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox,
    ConfigurationWindowDataManager *dataManager)
        : ProxyComboBox(parentConfigGroupBox->widget()),
          ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
    Q_UNUSED(itemValues)
    Q_UNUSED(itemCaptions)

    createWidgets();
}

ConfigProxyComboBox::ConfigProxyComboBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
        : ProxyComboBox(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
{
}

ConfigProxyComboBox::~ConfigProxyComboBox()
{
    if (label)
        delete label;
}

void ConfigProxyComboBox::setNetworkProxyManager(NetworkProxyManager *networkProxyManager)
{
    m_networkProxyManager = networkProxyManager;
}

void ConfigProxyComboBox::createWidgets()
{
    label = new QLabel(
        QCoreApplication::translate("@default", widgetCaption.toUtf8().constData()) + ':',
        parentConfigGroupBox->widget());
    parentConfigGroupBox->addWidgets(label, this);

    clear();
    insertItems(0, itemCaptions);

    if (!ConfigWidget::toolTip.isEmpty())
    {
        setToolTip(QCoreApplication::translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
        label->setToolTip(QCoreApplication::translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
    }
}

void ConfigProxyComboBox::loadConfiguration()
{
    if (!dataManager)
        return;

    setCurrentProxy(m_networkProxyManager->byUuid(dataManager->readEntry(section, item).toString()));

    emit activated(currentIndex());
}

void ConfigProxyComboBox::saveConfiguration()
{
    if (!dataManager)
        return;

    dataManager->writeEntry(section, item, currentProxy().uuid().toString());
}

void ConfigProxyComboBox::setVisible(bool visible)
{
    label->setVisible(visible);
    QComboBox::setVisible(visible);
}
