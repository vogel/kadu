/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "activate.h"
#include "debug.h"

#include "hints-configuration-window.h"

#include "hints_configuration_widget.h"

HintsConfigurationWidget::HintsConfigurationWidget(QWidget *parent)
	: NotifierConfigurationWidget(parent)
{
	preview = new QLabel(tr("<b>Here</b> you can see the preview"), this);
	preview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	QPushButton *configureButton = new QPushButton(tr("Configure"));
	connect(configureButton, SIGNAL(clicked()), this, SLOT(showConfigurationWindow()));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
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

	currentNotificationEvent = event;
	updatePreview();
}

void HintsConfigurationWidget::showConfigurationWindow()
{
	HintsConfigurationWindow *configWindow = HintsConfigurationWindow::configWindowForEvent(currentNotificationEvent);
	connect(configWindow, SIGNAL(configurationSaved()), this, SLOT(updatePreview()));

	configWindow->show();
}

void HintsConfigurationWidget::updatePreview()
{
	QFont font(qApp->font());
	QPalette palette(qApp->palette());

	preview->setFont(Application::instance()->configuration()->deprecatedApi()->readFontEntry("Hints", "Event_" + currentNotificationEvent + "_font", &font));

	QColor bcolor = Application::instance()->configuration()->deprecatedApi()->readColorEntry("Hints", "Event_" + currentNotificationEvent + "_bgcolor", &palette.window().color());
	QColor fcolor = Application::instance()->configuration()->deprecatedApi()->readColorEntry("Hints", "Event_" + currentNotificationEvent + "_fgcolor", &palette.windowText().color());
	QString style = QString("* {color:%1; background-color:%2}").arg(fcolor.name(), bcolor.name());
	preview->setStyleSheet(style);
}

#include "moc_hints_configuration_widget.cpp"
