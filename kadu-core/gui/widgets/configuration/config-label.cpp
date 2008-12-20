/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QtGui/QApplication>

#include "gui/widgets/configuration/config-label.h"
#include "gui/widgets/configuration/config-group-box.h"

ConfigLabel::ConfigLabel(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: QLabel(parentConfigGroupBox->widget(), name), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox, dataManager)
{
	createWidgets();
}

ConfigLabel::ConfigLabel(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: QLabel(parentConfigGroupBox->widget(), name), ConfigWidget(parentConfigGroupBox, dataManager)
{
}

void ConfigLabel::createWidgets()
{
	kdebugf();

	setText(qApp->translate("@default", widgetCaption));
	parentConfigGroupBox->addWidget(this);

	if (!ConfigWidget::toolTip.isEmpty())
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
}

void ConfigLabel::show()
{
	QLabel::show();
}

void ConfigLabel::hide()
{
	QLabel::hide();
}