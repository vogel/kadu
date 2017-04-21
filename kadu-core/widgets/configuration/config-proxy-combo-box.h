/*
 * %kadu copyright begin%
 * Copyright 2017, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "widgets/configuration/config-widget-value.h"
#include "widgets/proxy-combo-box.h"

class ConfigGroupBox;
class NetworkProxyManager;

class QLabel;
class QDomElement;

class KADUAPI ConfigProxyComboBox : public ProxyComboBox, public ConfigWidgetValue
{
    Q_OBJECT

public:
    ConfigProxyComboBox(
        const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
        const QStringList &itemValues, const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox,
        ConfigurationWindowDataManager *dataManager);
    ConfigProxyComboBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
    virtual ~ConfigProxyComboBox();

    virtual void setVisible(bool visible);

    virtual void loadConfiguration();
    virtual void saveConfiguration();

protected:
    virtual void createWidgets();

private:
    QPointer<NetworkProxyManager> m_networkProxyManager;

    QLabel *label;

    QStringList itemValues;
    QStringList itemCaptions;

private slots:
    INJEQT_SET void setNetworkProxyManager(NetworkProxyManager *networkProxyManager);
};
