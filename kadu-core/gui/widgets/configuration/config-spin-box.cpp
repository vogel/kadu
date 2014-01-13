/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-spin-box.h"

#include "debug.h"

ConfigSpinBox::ConfigSpinBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, const QString &specialValue,
		int minValue, int maxValue, int step, QString suffix, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QSpinBox(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager),
		label(0)
{
	if (!specialValue.isEmpty())
		setSpecialValueText(QCoreApplication::translate("@default", specialValue.toUtf8().constData()));
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
	delete label;
}

void ConfigSpinBox::createWidgets()
{
	kdebugf();

	label = new QLabel(QCoreApplication::translate("@default", widgetCaption.toUtf8().constData()) + ':', parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(QCoreApplication::translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
		label->setToolTip(QCoreApplication::translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
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

void ConfigSpinBox::setVisible(bool visible)
{
	label->setVisible(visible);
	QSpinBox::setVisible(visible);
}

bool ConfigSpinBox::fromDomElement(QDomElement domElement)
{
	QString minValue = domElement.attribute("min-value");
	QString maxValue = domElement.attribute("max-value");
	QString step = domElement.attribute("step");
	QString specialValue = domElement.attribute("special-value");
	Suffix = domElement.attribute("suffix");

	if (!specialValue.isEmpty())
		setSpecialValueText(QCoreApplication::translate("@default", specialValue.toUtf8().constData()));

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
		suffix = QCoreApplication::translate("@default", Suffix.toUtf8().constData(), 0, QCoreApplication::CodecForTr, i);
		QRegExp rx(QString("^.*%1").arg(i));
		rx.setMinimal(true);
		suffix.remove(rx);
	}
	setSuffix(suffix);
}

#include "moc_config-spin-box.cpp"
