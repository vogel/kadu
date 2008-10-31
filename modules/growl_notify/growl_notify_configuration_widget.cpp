/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QApplication>

#include "kadu.h"
#include "config_file.h"
#include "debug.h"

#include "growl_notify_configuration_widget.h"

GrowlNotifyConfigurationWidget::GrowlNotifyConfigurationWidget(QWidget *parent, char *name)
	: NotifierConfigurationWidget(parent, name), currentNotifyEvent("")
{
	QString tooltip = qApp->translate("@default", Kadu::SyntaxTextNotify) + 
		tr("\n%&t - title (eg. New message) %&m - notification text (eg. Message from Jim), %&d - details (eg. message quotation),\n%&i - notification icon");

	title = new QLineEdit(this);
	title->setToolTip(tooltip);

	syntax = new QLineEdit(this);
	syntax->setToolTip(tooltip);

	connect(timeout, SIGNAL(valueChanged(int)), this, SLOT(timeoutChanged(int)));
	connect(syntax, SIGNAL(textChanged(const QString &)), this, SLOT(syntaxChanged(const QString &)));
	connect(title, SIGNAL(textChanged(const QString &)), this, SLOT(titleChanged(const QString &)));

	QGridLayout *gridLayout = new QGridLayout(this, 0, 0, 0, 5);
	gridLayout->addWidget(new QLabel(tr("Title") + ":", this), 0, 0, Qt::AlignRight);
	gridLayout->addWidget(title, 0, 1);
	gridLayout->addWidget(new QLabel(tr("Syntax") + ":", this), 1, 0, Qt::AlignRight);
	gridLayout->addWidget(syntax, 1, 1);

	parent->layout()->addWidget(this);
}

void GrowlNotifyConfigurationWidget::saveNotifyConfigurations()
{
	kdebugf();

	if (currentNotifyEvent != "")
		properties[currentNotifyEvent] = currentProperties;

	foreach(const GrowlNotifyProperties &property, properties)
	{
		const QString &eventName = property.eventName;

		config_file.writeEntry("GrowlNotify", QString("Event_") + eventName + "_syntax", property.syntax);
		config_file.writeEntry("GrowlNotify", QString("Event_") + eventName + "_title", property.title);
	}
}

void GrowlNotifyConfigurationWidget::switchToEvent(const QString &event)
{
	kdebugf();

	if (currentNotifyEvent != "")
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
	}

	syntax->setText(currentProperties.syntax);
	title->setText(currentProperties.title);
}

void GrowlNotifyConfigurationWidget::syntaxChanged(const QString &syntax)
{
	currentProperties.syntax = syntax;
}

void GrowlNotifyConfigurationWidget::titleChanged(const QString &title)
{
	currentProperties.title = title;
}
