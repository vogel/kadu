/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2010 Tomasz Rostański (rozteck@interia.pl)
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

#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QGridLayout>
#include <QtGui/QApplication>

#include "debug.h"
#include "configuration/configuration-file.h"

#include "growl_notify_configuration_widget.h"

GrowlNotifyConfigurationWidget::GrowlNotifyConfigurationWidget(QWidget *parent)
	: NotifierConfigurationWidget(parent), currentNotifyEvent("")
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

	if (!currentNotifyEvent.isEmpty())
		properties[currentNotifyEvent] = currentProperties;

	foreach(const GrowlNotifyProperties &property, properties)
	{
		const QString &eventName = property.eventName;

		config_file.writeEntry("GrowlNotify", QString("Event_") + eventName + "_syntax", property.syntax);
		config_file.writeEntry("GrowlNotify", QString("Event_") + eventName + "_title", property.title);
//		config_file.writeEntry("GrowlNotify", QString("Event_") + eventName + "_avatar", property.showAvatar);
	}
}

void GrowlNotifyConfigurationWidget::switchToEvent(const QString &event)
{
	kdebugf();

	if (!currentNotifyEvent.isEmpty())
		properties[currentNotifyEvent] = currentProperties;

	if (properties.contains(event))
	{
		currentProperties = properties[event];
		currentNotifyEvent = event;
	}
	else
	{
		currentNotifyEvent = event;
		currentProperties.eventName = event;

		currentProperties.syntax = config_file.readEntry("GrowlNotify", QString("Event_") + event + "_syntax");
		currentProperties.title = config_file.readEntry("GrowlNotify", QString("Event_") + event + "_title");
//		currentProperties.showAvatar = config_file.readBoolEntry("GrowlNotify", QString("Event_") + event + "_avatar");
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
