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

ConfigLabel::ConfigLabel(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QLabel(parentConfigGroupBox->widget()), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox, dataManager)
{
	createWidgets();
}

ConfigLabel::ConfigLabel(ConfigGroupBox *p0arentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QLabel(parentConfigGroupBox->widget()), ConfigWidget(parentConfigGroupBox, dataManager)
{
}

void ConfigLabel::createWidgets()
{
	kdebugf();

	setText(qApp->tr("@default", widgetCaption.toAscii().data()));
	parentConfigGroupBox->addWidget(this);

	if (!ConfigWidget::toolTip.isEmpty())
		setToolTip(qApp->tr("@default", ConfigWidget::toolTip.toAscii().data()));
}

void ConfigLabel::show()
{
	QLabel::show();
}

void ConfigLabel::hide()
{
	QLabel::hide();
}
