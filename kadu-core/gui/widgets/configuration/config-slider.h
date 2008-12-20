/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_SLIDER_H
#define CONFIG_SLIDER_H

#include <QtGui/QSlider>

#include "gui/widgets/configuration/config-widget-value.h"
#include "configuration/configuration-window-data-manager.h"

class QLabel;
class QDomElement;
class ConfigGroupBox;

/**
	&lt;slider caption="caption" id="id" /&gt;

	@arg minValue - minimalna wartość (obowiązkowo)
	@arg maxValue - maksymalna wartość (obowiązkowo)
	@arg pageStep - krok wartości (obowiązkowo)
 **/
class KADUAPI ConfigSlider : public QSlider, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSlider(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		int minValue, int maxValue, int pageStep, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name);
	ConfigSlider(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigSlider();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

#endif