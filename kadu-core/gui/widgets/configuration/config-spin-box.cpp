/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#include "gui/widgets/configuration/config-spin-box.h"
#include "gui/widgets/configuration/config-group-box.h"

#include "debug.h"

ConfigSpinBox::ConfigSpinBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, const QString &specialValue,
		int minValue, int maxValue, int step, QString suffix, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QSpinBox(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager),
		label(0)
{
	if (!specialValue.isEmpty())
		setSpecialValueText(qApp->translate("@default", specialValue.toAscii().data()));
	setMinimum(minValue);
	setMaximum(maxValue);
	setSingleStep(step);
	Suffix = suffix;
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}

ConfigSpinBox::ConfigSpinBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QSpinBox(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
{
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}

ConfigSpinBox::~ConfigSpinBox()
{
	disconnect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
	if (label)
		delete label;
}

void ConfigSpinBox::createWidgets()
{
	kdebugf();

	label = new QLabel(qApp->translate("@default", widgetCaption.toAscii().data()) + ':', parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toAscii().data()));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toAscii().data()));
	}
}

void ConfigSpinBox::loadConfiguration()
{
	if (!dataManager)
		return;
	setValue(dataManager->readEntry(section, item).toInt());
	emit valueChanged(value());
}

void ConfigSpinBox::saveConfiguration()
{
	if (!dataManager)
		return;
	dataManager->writeEntry(section, item, QVariant(value()));
}

void ConfigSpinBox::show()
{
	label->show();
	QSpinBox::show();
}

void ConfigSpinBox::hide()
{
	label->hide();
	QSpinBox::hide();
}

bool ConfigSpinBox::fromDomElement(QDomElement domElement)
{
	QString minValue = domElement.attribute("min-value");
	QString maxValue = domElement.attribute("max-value");
	QString step = domElement.attribute("step");
	QString specialValue = domElement.attribute("special-value");
	Suffix = domElement.attribute("suffix");

	if (!specialValue.isEmpty())
		setSpecialValueText(qApp->translate("@default", specialValue.toAscii().data()));

	bool ok;

	setMinimum(minValue.toInt(&ok));
	if (!ok)
		return false;

	setMaximum(maxValue.toInt(&ok));
	if (!ok)
		return false;

	setSingleStep(step.toInt(&ok));
	if (!ok)
		setSingleStep(1);

	return ConfigWidgetValue::fromDomElement(domElement);
}

void ConfigSpinBox::onValueChanged(int i)
{
	QString suffix = Suffix;
	if (Suffix.contains("%n"))
	{
		suffix = qApp->translate("@default", Suffix.toAscii().data(), 0, QCoreApplication::CodecForTr, i);
		QRegExp rx(QString("^.*%1").arg(i));
		rx.setMinimal(true);
		suffix.remove(rx);
	}
	setSuffix(suffix);
}
