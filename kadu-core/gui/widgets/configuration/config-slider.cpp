/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

	label = new QLabel(qApp->translate("@default", widgetCaption.toUtf8().constData()) + ':', parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
	}
}

void ConfigSlider::loadConfiguration()
{
	if (!dataManager)
		return;
	setValue(dataManager->readEntry(section, item).toInt());
	emit valueChanged(value());
}

void ConfigSlider::saveConfiguration()
{
	if (!dataManager)
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
