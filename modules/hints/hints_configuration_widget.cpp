/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/misc.h"
#include "activate.h"
#include "debug.h"

#include "hints-configuration-window.h"

#include "hints_configuration_widget.h"

HintsConfigurationWidget::HintsConfigurationWidget(QWidget *parent)
	: NotifierConfigurationWidget(parent), currentNotifyEvent("")
{
	preview = new QLabel(tr("<b>Preview</b> text"), this);
    	QPushButton *configureButton = new QPushButton(tr("Configure"));
	connect(configureButton, SIGNAL(clicked()), this, SLOT(showConfigurationWindow()));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(preview);
	layout->addWidget(configureButton);

	dynamic_cast<NotifyGroupBox *>(parent)->addWidget(this);
}

void HintsConfigurationWidget::saveNotifyConfigurations()
{
}

void HintsConfigurationWidget::switchToEvent(const QString &event)
{
	kdebugf();

	currentNotifyEvent = event;
	updatePreview();
}

void HintsConfigurationWidget::showConfigurationWindow()
{
	HintsConfigurationWindow *configWindow = HintsConfigurationWindow::configWindowForEvent(currentNotifyEvent);
	connect(configWindow, SIGNAL(configurationSaved()), this, SLOT(updatePreview()));

	configWindow->show();
}

void HintsConfigurationWidget::updatePreview()
{
	preview->setFont(config_file.readFontEntry("Hints", "Event_" + currentNotifyEvent + "_font"));

	QColor bcolor = config_file.readColorEntry("Hints", "Event_" + currentNotifyEvent + "_bgcolor");
	QColor fcolor = config_file.readColorEntry("Hints", "Event_" + currentNotifyEvent + "_fgcolor");
	QString style = narg("QWidget {color:%1; background-color:%2}", fcolor.name(), bcolor.name());
	preview->setStyleSheet(style);
}
