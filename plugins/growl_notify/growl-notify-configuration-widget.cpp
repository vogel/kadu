/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "debug.h"

#include "growl-notify-configuration-widget.h"

GrowlNotifyConfigurationWidget::GrowlNotifyConfigurationWidget(QWidget *parent)
	: NotifierConfigurationWidget(parent), currentNotificationEvent("")
{
	QString tooltip = tr("\n%&t - title (eg. New message) %&m - notification text (eg. Message from Jim), %&d - details (eg. message quotation),\n%&i - notification icon");

	title = new QLineEdit(this);
	title->setToolTip(tooltip);

	syntax = new QLineEdit(this);
	syntax->setToolTip(tooltip);

//	showAvatar = new QCheckBox();

	connect(syntax, SIGNAL(textChanged(const QString &)), this, SLOT(syntaxChanged(const QString &)));
	connect(title, SIGNAL(textChanged(const QString &)), this, SLOT(titleChanged(const QString &)));
//	connect(showAvatar, SIGNAL(stateChanged(int)), this, SLOT(avatarChanged(int)));

	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->addWidget(new QLabel(tr("Title") + ':', this), 0, 0, Qt::AlignRight);
	gridLayout->addWidget(title, 0, 1);
	gridLayout->addWidget(new QLabel(tr("Syntax") + ':', this), 1, 0, Qt::AlignRight);
	gridLayout->addWidget(syntax, 1, 1);
//	gridLayout->addWidget(new QLabel(tr("Display avatar if available"), this), 2, 0, Qt::AlignRight);
//	gridLayout->addWidget(showAvatar, 2, 1);

	parent->layout()->addWidget(this);
}

void GrowlNotifyConfigurationWidget::saveNotifyConfigurations()
{
	kdebugf();

	if (!currentNotificationEvent.isEmpty())
		properties[currentNotificationEvent] = currentProperties;

	foreach(const GrowlNotifyProperties &property, properties)
	{
		const QString &eventName = property.eventName;

		Application::instance()->configuration()->deprecatedApi()->writeEntry("GrowlNotify", QString("Event_") + eventName + "_syntax", property.syntax);
		Application::instance()->configuration()->deprecatedApi()->writeEntry("GrowlNotify", QString("Event_") + eventName + "_title", property.title);
//		Application::instance()->configuration()->deprecatedApi()->writeEntry("GrowlNotify", QString("Event_") + eventName + "_avatar", property.showAvatar);
	}
}

void GrowlNotifyConfigurationWidget::switchToEvent(const QString &event)
{
	kdebugf();

	if (!currentNotificationEvent.isEmpty())
		properties[currentNotificationEvent] = currentProperties;

	if (properties.contains(event))
	{
		currentProperties = properties[event];
		currentNotificationEvent = event;
	}
	else
	{
		currentNotificationEvent = event;
		currentProperties.eventName = event;

		currentProperties.syntax = Application::instance()->configuration()->deprecatedApi()->readEntry("GrowlNotify", QString("Event_") + event + "_syntax");
		currentProperties.title = Application::instance()->configuration()->deprecatedApi()->readEntry("GrowlNotify", QString("Event_") + event + "_title");
//		currentProperties.showAvatar = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("GrowlNotify", QString("Event_") + event + "_avatar");
	}

	syntax->setText(currentProperties.syntax);
	title->setText(currentProperties.title);
//	showAvatar->setCheckState(currentProperties.showAvatar ? Qt::Checked : Qt::Unchecked);
}

void GrowlNotifyConfigurationWidget::syntaxChanged(const QString &syntax)
{
	currentProperties.syntax = syntax;
}

void GrowlNotifyConfigurationWidget::titleChanged(const QString &title)
{
	currentProperties.title = title;
}

//void GrowlNotifyConfigurationWidget::avatarChanged(int state)
//{
//	currentProperties.showAvatar = (state == Qt::Checked);
//}

#include "moc_growl-notify-configuration-widget.cpp"
