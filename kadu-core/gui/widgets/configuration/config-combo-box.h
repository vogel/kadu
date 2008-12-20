/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_COMBO_BOX_H
#define CONFIG_COMBO_BOX_H

#include <QtGui/QComboBox>

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
	QLabel *label;

	QStringList itemValues;
	QStringList itemCaptions;

protected:
	virtual void createWidgets();

public:
	ConfigComboBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, const QStringList &itemValues, const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name = 0);
	ConfigComboBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name = 0);
	virtual ~ConfigComboBox();

	void setItems(const QStringList &itemValues, const QStringList &itemCaptions);
	QString currentItemValue();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

#endif 