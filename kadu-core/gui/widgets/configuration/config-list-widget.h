/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_LIST_WIDGET_H
#define CONFIG_LIST_WIDGET_H

#include <QtGui/QListWidget>

#include "gui/widgets/configuration/config-widget.h"

class QLabel;
class QDomElement;
class ConfigGroupBox;
class ConfigurationWindowDataManager;

/**
	&lt;list-box caption="caption" id="id"&gt;
		&lt;item value="value" caption="caption"&gt;
		...
	&lt;/list-box&gt;

	@arg value - wartość zapisana do pliku konfiguracyjnego
	@arg caption - wartość wyświetlana
 **/
class KADUAPI ConfigListWidget : public QListWidget, public ConfigWidget
{
	QLabel *label;

	QStringList itemValues;
	QStringList itemCaptions;

protected:
	virtual void createWidgets();

public:
	ConfigListWidget(const QString &widgetCaption, const QString &toolTip,
		const QStringList &itemValues, const QStringList &itemCaptions,
		ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name = 0);
	ConfigListWidget(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name = 0);
	virtual ~ConfigListWidget();

	void setItems(const QStringList &itemValues, const QStringList &itemCaptions);
	QString currentItemValue() { return itemValues[currentRow()]; }

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

#endif
