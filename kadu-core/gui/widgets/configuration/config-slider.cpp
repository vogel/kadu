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
#include <QtXml/QDomElement>

#include "gui/widgets/configuration/config-slider.h"
#include "gui/widgets/configuration/config-group-box.h"

#include "debug.h"

ConfigSlider::ConfigSlider(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		int minValue, int maxValue, int pageStep, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QSlider(Qt::Horizontal, parentConfigGroupBox->widget()),
		ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	setMinimum(minValue);
	setMaximum(maxValue);
	setPageStep(pageStep);
}

ConfigSlider::ConfigSlider(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QSlider(Qt::Horizontal, parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
{
}

ConfigSlider::~ConfigSlider()
{
	if (label)
		delete label;
}

void ConfigSlider::createWidgets()
{
	kdebugf();

	label = new QLabel(qApp->translate("@default", widgetCaption.toAscii().data()) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toAscii().data()));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toAscii().data()));
	}
}

void ConfigSlider::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setValue(dataManager->readEntry(section, item).toInt());
	emit valueChanged(value());
}

void ConfigSlider::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, QString::number(value()));
}

void ConfigSlider::show()
{
	label->show();
	QSlider::show();
}

void ConfigSlider::hide()
{
	label->hide();
	QSlider::hide();
}

bool ConfigSlider::fromDomElement(QDomElement domElement)
{
	QString minValue = domElement.attribute("min-value");
	QString maxValue = domElement.attribute("max-value");
	QString pageStep = domElement.attribute("page-step");

	bool ok;

	setMinimum(minValue.toInt(&ok));
	if (!ok)
		return false;

	setMaximum(maxValue.toInt(&ok));
	if (!ok)
		return false;

	setPageStep(pageStep.toInt(&ok));
	if (!ok)
		return false;

	return ConfigWidgetValue::fromDomElement(domElement);
}
