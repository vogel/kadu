/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qspinbox.h>

#include "color_button.h"
#include "config_file.h"
#include "debug.h"
#include "select_font.h"

#include "hints_configuration_widget.h"

HintsConfigurationWidget::HintsConfigurationWidget(QWidget *parent, char *name)
	: NotifierConfigurationWidget(parent, name), currentNotifyEvent("")
{
	preview = new QLabel(tr("<b>Preview</b> text"), this);
	font = new SelectFont(this);
	foregroundColor = new ColorButton(this);
	backgroundColor = new ColorButton(this);
	timeout = new QSpinBox(this);
	syntax = new QLineEdit(this);

	connect(font, SIGNAL(fontChanged(QFont)), this, SLOT(fontChanged(QFont)));
	connect(foregroundColor, SIGNAL(changed(const QColor &)), this, SLOT(foregroundColorChanged(const QColor &)));
	connect(backgroundColor, SIGNAL(changed(const QColor &)), this, SLOT(backgroundColorChanged(const QColor &)));
	connect(timeout, SIGNAL(valueChanged(int)), this, SLOT(timeoutChanged(int)));
	connect(syntax, SIGNAL(textChanged(const QString &)), this, SLOT(syntaxChanged(const QString &)));

	QGridLayout *gridLayout = new QGridLayout(this, 0, 0, 0, 5);
	gridLayout->addMultiCellWidget(preview, 0, 0, 0, 1);
	gridLayout->addWidget(new QLabel(tr("Font") + ":", this), 1, 0, Qt::AlignRight);
	gridLayout->addWidget(font, 1, 1);
	gridLayout->addWidget(new QLabel(tr("Font color") + ":", this), 2, 0, Qt::AlignRight);
	gridLayout->addWidget(foregroundColor, 2, 1);
	gridLayout->addWidget(new QLabel(tr("Background color") + ":", this), 3, 0, Qt::AlignRight);
	gridLayout->addWidget(backgroundColor, 3, 1);
	gridLayout->addWidget(new QLabel(tr("Timeout") + ":", this), 4, 0, Qt::AlignRight);
	gridLayout->addWidget(timeout, 4, 1);
	gridLayout->addWidget(new QLabel(tr("Syntax") + ":", this), 5, 0, Qt::AlignRight);
	gridLayout->addWidget(syntax, 5, 1);
}

void HintsConfigurationWidget::saveNotifyConfigurations()
{
	kdebugf();

	if (currentNotifyEvent != "")
		hintProperties[currentNotifyEvent] = currentProperties;

	CONST_FOREACH(hintProperty, hintProperties)
	{
		const QString &eventName = (*hintProperty).eventName;

		config_file.writeEntry("Hints", QString("Event_") + eventName + "_font", (*hintProperty).font);
		config_file.writeEntry("Hints", QString("Event_") + eventName + "_fgcolor", (*hintProperty).foregroundColor);
		config_file.writeEntry("Hints", QString("Event_") + eventName + "_bgcolor", (*hintProperty).backgroundColor);
		config_file.writeEntry("Hints", QString("Event_") + eventName + "_timeout", (int)(*hintProperty).timeout);
		config_file.writeEntry("Hints", QString("Event_") + eventName + "_syntax", (*hintProperty).syntax);
	}
}

void HintsConfigurationWidget::switchToEvent(const QString &event)
{
	kdebugf();

	if (currentNotifyEvent != "")
		hintProperties[currentNotifyEvent] = currentProperties;

	if (hintProperties.contains(event))
	{
		currentProperties = hintProperties[event];
		currentNotifyEvent = event;
	}
	else
	{
		static QColor fgDefaultColor(0x00, 0x00, 0x00);
		static QColor bgDefaultColor(0xf0, 0xf0, 0xf0);

		currentNotifyEvent = event;
		currentProperties.eventName = event;

		currentProperties.font = config_file.readFontEntry("Hints", QString("Event_") + event + "_font");
		currentProperties.foregroundColor = config_file.readColorEntry("Hints", QString("Event_") + event+"_fgcolor", &fgDefaultColor);
		currentProperties.backgroundColor = config_file.readColorEntry("Hints", QString("Event_") + event + "_bgcolor", &bgDefaultColor);
		currentProperties.timeout = config_file.readUnsignedNumEntry("Hints", QString("Event_") + event + "_timeout", 10);
		currentProperties.syntax = config_file.readEntry("Hints", QString("Event_") + event + "_syntax");
	}

	font->setFont(currentProperties.font);
	foregroundColor->setColor(currentProperties.foregroundColor);
	backgroundColor->setColor(currentProperties.backgroundColor);
	timeout->setValue(currentProperties.timeout);

	preview->setFont(currentProperties.font);
	preview->setPaletteForegroundColor(currentProperties.foregroundColor);
	preview->setPaletteBackgroundColor(currentProperties.backgroundColor);
}

void HintsConfigurationWidget::fontChanged(QFont font)
{
	currentProperties.font = font;
	preview->setFont(font);
}

void HintsConfigurationWidget::foregroundColorChanged(const QColor &color)
{
	currentProperties.foregroundColor = color;
	preview->setPaletteForegroundColor(color);
}

void HintsConfigurationWidget::backgroundColorChanged(const QColor &color)
{
	currentProperties.backgroundColor = color;
	preview->setPaletteBackgroundColor(color);
}

void HintsConfigurationWidget::timeoutChanged(int timeout)
{
	currentProperties.timeout = timeout;
}

void HintsConfigurationWidget::syntaxChanged(const QString &syntax)
{
	currentProperties.syntax = syntax;
}
