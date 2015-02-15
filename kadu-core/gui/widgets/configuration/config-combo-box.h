/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONFIG_COMBO_BOX_H
#define CONFIG_COMBO_BOX_H

#include <QtWidgets/QComboBox>

#include "gui/widgets/configuration/config-widget-value.h"

class QLabel;
class QDomElement;
class ConfigGroupBox;

/**
	&lt;combo-box caption="caption" id="id"&gt;
		&lt;item value="value" caption="caption"&gt;
		...
	&lt;/combo-box&gt;

	@arg value - wartość zapisana do pliku konfiguracyjnego
	@arg caption - wartość wyświetlana
 **/
class KADUAPI ConfigComboBox : public QComboBox, public ConfigWidgetValue
{
	Q_OBJECT

	QLabel *label;

	bool saveIndexNotCaption;

	QStringList itemValues;
	QStringList itemCaptions;

protected:
	virtual void createWidgets();

public:
	ConfigComboBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, const QStringList &itemValues, const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigComboBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigComboBox();

	void setItems(const QStringList &itemValues, const QStringList &itemCaptions);
	void setCurrentItem(const QString &value);
	QString currentItemValue();

	virtual void setVisible(bool visible);

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual bool fromDomElement(QDomElement domElement);
};

#endif
