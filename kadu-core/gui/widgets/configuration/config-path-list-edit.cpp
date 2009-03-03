/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QLabel>

#include "gui/widgets/configuration/config-path-list-edit.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "configuration/configuration-window-data-manager.h"

ConfigPathListEdit::ConfigPathListEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: PathListEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigPathListEdit::ConfigPathListEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: PathListEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
{
}

ConfigPathListEdit::~ConfigPathListEdit()
{
	if (label)
		delete label;
}

void ConfigPathListEdit::createWidgets()
{
	kdebugf();

	label = new QLabel(qApp->tr("@default", widgetCaption.toAscii().data()) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->tr("@default", ConfigWidget::toolTip.toAscii().data()));
		label->setToolTip(qApp->tr("@default", ConfigWidget::toolTip.toAscii().data()));
	}
}

void ConfigPathListEdit::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setPathList(dataManager->readEntry(section, item).toString().split(QRegExp("&")));
}

void ConfigPathListEdit::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, QVariant(pathList().join("&")));
}

void ConfigPathListEdit::show()
{
	label->show();
	PathListEdit::show();
}

void ConfigPathListEdit::hide()
{
	label->hide();
	PathListEdit::hide();
}
