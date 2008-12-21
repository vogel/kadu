/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_SPIN_BOX_H
#define CONFIG_SPIN_BOX_H

#include <QtGui/QSpinBox>

#include "gui/widgets/configuration/config-widget-value.h"
#include "configuration/configuration-window-data-manager.h"

class QLabel;
class QDomElement;
class ConfigGroupBox;

/**
	&lt;spin-box caption="caption" id="id" min-value="minValue" max-value="maxValue" step="step" /&gt;

	@arg minValue - minimalna wartość (obowiązkowo)
	@arg maxValue - maksymalna wartość (obowiązkowo)
	@arg step - krok wartości (nieobowiazkowo, domyślnie 1)
 **/
class KADUAPI ConfigSpinBox : public QSpinBox, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSpinBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, int minValue, int maxValue, int step, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name = 0);
	ConfigSpinBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name = 0);
	virtual ~ConfigSpinBox();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

#endif
