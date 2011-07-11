/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
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

#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "activate.h"
#include "debug.h"

#include "hints-configuration-window.h"

#include "hints_configuration_widget.h"

HintsConfigurationWidget::HintsConfigurationWidget(QWidget *parent)
	: NotifierConfigurationWidget(parent)
{
	preview = new QLabel(tr("<b>Preview</b> text"), this);
	preview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	preview->setMargin(3);

	QPushButton *configureButton = new QPushButton(tr("Configure"));
	connect(configureButton, SIGNAL(clicked()), this, SLOT(showConfigurationWindow()));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(preview);
	layout->addWidget(configureButton);

	static_cast<NotifyGroupBox *>(parent)->addWidget(this);
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
	QFont font(qApp->font());
	QPalette palette(qApp->palette());

	preview->setFont(config_file.readFontEntry("Hints", "Event_" + currentNotifyEvent + "_font", &font));

	QColor bcolor = config_file.readColorEntry("Hints", "Event_" + currentNotifyEvent + "_bgcolor", &palette.window().color());
	QColor fcolor = config_file.readColorEntry("Hints", "Event_" + currentNotifyEvent + "_fgcolor", &palette.windowText().color());
	QString style = QString("* {color:%1; background-color:%2}").arg(fcolor.name(), bcolor.name());
	preview->setStyleSheet(style);
}
