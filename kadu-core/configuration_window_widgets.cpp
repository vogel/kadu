/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qdom.h>
#include <qlabel.h>
#include <qregexp.h>

#include "configuration_window.h"
#include "config_file.h"
#include "debug.h"

#include "configuration_window_widgets.h"

ConfigWidget::ConfigWidget(ConfigGroupBox *parentConfigGroupBox)
	: parentConfigGroupBox(parentConfigGroupBox)
{
}

ConfigWidget::ConfigWidget(const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox)
	: parentConfigGroupBox(parentConfigGroupBox), widgetCaption(widgetCaption)
{
}

bool ConfigWidget::fromDomElement(QDomElement domElement)
{
	widgetCaption = domElement.attribute("caption");

	if (widgetCaption.isEmpty())
		return false;

	createWidgets();
	return true;
}

ConfigWidgetValue::ConfigWidgetValue(ConfigGroupBox *parentConfigGroupBox)
	: ConfigWidget(parentConfigGroupBox)
{
}

ConfigWidgetValue::ConfigWidgetValue(const QString &widgetCaption, const QString &section, const QString &item, ConfigGroupBox *parentConfigGroupBox)
	: ConfigWidget(widgetCaption, parentConfigGroupBox), section(section), item(item)
{
}

bool ConfigWidgetValue::fromDomElement(QDomElement domElement)
{
	section = domElement.attribute("config-section");
	item = domElement.attribute("config-item");

	if (section.isEmpty() || item.isEmpty())
		return false;

	return ConfigWidget::fromDomElement(domElement);
}

ConfigLineEdit::ConfigLineEdit(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: QLineEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, section, item, parentConfigGroupBox)
{
	createWidgets();
}

ConfigLineEdit::ConfigLineEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: QLineEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigLineEdit::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigLineEdit::loadConfiguration()
{
	setText(config_file.readEntry(section, item));
}

void ConfigLineEdit::saveConfiguration()
{
	config_file.writeEntry(section, item, text());
}

ConfigGGPasswordEdit::ConfigGGPasswordEdit(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: ConfigLineEdit(section, item, widgetCaption, parentConfigGroupBox, name)
{
	setEchoMode(QLineEdit::Password);
}

ConfigGGPasswordEdit::ConfigGGPasswordEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: ConfigLineEdit(parentConfigGroupBox, name)
{
	setEchoMode(QLineEdit::Password);
}

void ConfigGGPasswordEdit::loadConfiguration()
{
	setText(pwHash(config_file.readEntry(section, item)));
}

void ConfigGGPasswordEdit::saveConfiguration()
{
	config_file.writeEntry(section, item, pwHash(text()));
}

ConfigCheckBox::ConfigCheckBox(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: QCheckBox(widgetCaption, parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, section, item, parentConfigGroupBox)
{
	createWidgets();
}

ConfigCheckBox::ConfigCheckBox(ConfigGroupBox *parentConfigGroupBox, char *name)
	: QCheckBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigCheckBox::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();

	setText(widgetCaption);

	int numRows = layout->numRows();
	layout->addMultiCellWidget(this, numRows, numRows, 0, 1);
}

void ConfigCheckBox::loadConfiguration()
{
	setChecked(config_file.readBoolEntry(section, item));
	emit toggled(isChecked());
}

void ConfigCheckBox::saveConfiguration()
{
	config_file.writeEntry(section, item, isChecked());
}

ConfigSpinBox::ConfigSpinBox(const QString &section, const QString &item, const QString &widgetCaption,
		int minValue, int maxValue, int step, ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QSpinBox(minValue, maxValue, step, parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, section, item, parentConfigGroupBox)
{
}

ConfigSpinBox::ConfigSpinBox(ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QSpinBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigSpinBox::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, widgetCaption + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigSpinBox::loadConfiguration()
{
	setValue(config_file.readNumEntry(section, item));
}

void ConfigSpinBox::saveConfiguration()
{
	config_file.writeEntry(section, item, value());
}


bool ConfigSpinBox::fromDomElement(QDomElement domElement)
{
	QString minValue = domElement.attribute("min-value");
	QString maxValue = domElement.attribute("max-value");
	QString step = domElement.attribute("step");
	setSuffix(domElement.attribute("suffix"));

	bool ok;

	setMinValue(minValue.toInt(&ok));
	if (!ok)
		return false;

	setMaxValue(maxValue.toInt(&ok));
	if (!ok)
		return false;

	setLineStep(step.toInt(&ok));
	if (!ok)
		return false;

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigComboBox::ConfigComboBox(const QString &section, const QString &item, const QString &widgetCaption, const QStringList &itemValues,
	const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QComboBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, section, item, parentConfigGroupBox)
{
	createWidgets();
}

ConfigComboBox::ConfigComboBox(ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QComboBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigComboBox::setItems(const QStringList &itemValues, const QStringList &itemCaptions)
{
	this->itemValues = itemValues;
	this->itemCaptions = itemCaptions;

	clear();
	insertStringList(itemCaptions);
}

void ConfigComboBox::createWidgets()
{
	kdebugf();

	connect(this, SIGNAL(activatd(int index)), this, SLOT(activatedSlot(int index)));

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, widgetCaption + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);

	clear();
	insertStringList(itemCaptions);
}

void ConfigComboBox::loadConfiguration()
{
	QString currentValue = config_file.readEntry(section, item);
	setCurrentItem(itemValues.findIndex(currentValue));

	emit activated(currentItem());
}

void ConfigComboBox::saveConfiguration()
{
	config_file.writeEntry(section, item, itemValues[currentItem()]);
}

bool ConfigComboBox::fromDomElement(QDomElement domElement)
{
	QDomNodeList children = domElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
	{
		QDomNode node = children.item(i);
		if (node.isElement())
		{
			QDomElement element = node.toElement();
			if (element.tagName() != "item")
				continue;

			itemValues.append(element.attribute("value"));
			itemCaptions.append(element.attribute("caption"));

			insertItem(tr(element.attribute("caption")));
		}
	}

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigHotKeyEdit::ConfigHotKeyEdit(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: HotKeyEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, section, item, parentConfigGroupBox)
{
	createWidgets();
}

ConfigHotKeyEdit::ConfigHotKeyEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: HotKeyEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigHotKeyEdit::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigHotKeyEdit::loadConfiguration()
{
	setShortCut(config_file.readEntry(section, item));
}

void ConfigHotKeyEdit::saveConfiguration()
{
	config_file.writeEntry(section, item, shortCutString());
}

ConfigPathListEdit::ConfigPathListEdit(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: PathListEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, section, item, parentConfigGroupBox)
{
	createWidgets();
}

ConfigPathListEdit::ConfigPathListEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: PathListEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigPathListEdit::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigPathListEdit::loadConfiguration()
{
	setPathList(QStringList::split(QRegExp("(;|:)"), config_file.readEntry(section, item)));
}

void ConfigPathListEdit::saveConfiguration()
{
	config_file.writeEntry(section, item, pathList().join(":"));
}

ConfigColorButton::ConfigColorButton(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: ColorButton(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, section, item, parentConfigGroupBox)
{
	createWidgets();
}

ConfigColorButton::ConfigColorButton(ConfigGroupBox *parentConfigGroupBox, char *name)
	: ColorButton(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigColorButton::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigColorButton::loadConfiguration()
{
	setColor(config_file.readColorEntry(section, item));
}

void ConfigColorButton::saveConfiguration()
{
	config_file.writeEntry(section, item, color());
}

ConfigSelectFont::ConfigSelectFont(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: SelectFont(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, section, item, parentConfigGroupBox)
{
	createWidgets();
}

ConfigSelectFont::ConfigSelectFont(ConfigGroupBox *parentConfigGroupBox, char *name)
	: SelectFont(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigSelectFont::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigSelectFont::loadConfiguration()
{
	setFont(config_file.readFontEntry(section, item));
}

void ConfigSelectFont::saveConfiguration()
{
	config_file.writeEntry(section, item, font());
}

ConfigSyntaxEditor::ConfigSyntaxEditor(const QString &section, const QString &item, const QString &widgetCaption,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: SyntaxEditor(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, section, item, parentConfigGroupBox)
{
	createWidgets();
}

ConfigSyntaxEditor::ConfigSyntaxEditor(ConfigGroupBox *parentConfigGroupBox, char *name)
	: SyntaxEditor(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigSyntaxEditor::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigSyntaxEditor::loadConfiguration()
{
	setCurrentSyntax(config_file.readEntry(section, item));
}

void ConfigSyntaxEditor::saveConfiguration()
{
	config_file.writeEntry(section, item, currentSyntax());
}

bool ConfigSyntaxEditor::fromDomElement(QDomElement domElement)
{
	QString category = domElement.attribute("category");
	if (category.isEmpty())
		return false;

	setCategory(category);

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigActionButton::ConfigActionButton(const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: QPushButton(parentConfigGroupBox->widget(), name), ConfigWidget(widgetCaption, parentConfigGroupBox)
{
	createWidgets();
}

ConfigActionButton::ConfigActionButton(ConfigGroupBox *parentConfigGroupBox, char *name)
	: QPushButton(parentConfigGroupBox->widget(), name), ConfigWidget(parentConfigGroupBox)
{
}

void ConfigActionButton::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	setText(widgetCaption);
	layout->addWidget(this, numRows, 0, Qt::AlignRight);
}

ConfigSelectFile::ConfigSelectFile(const QString &section, const QString &item, const QString &widgetCaption, const QString &type,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: SelectFile(type, parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, section, item, parentConfigGroupBox)
{
	createWidgets();
}

ConfigSelectFile::ConfigSelectFile(ConfigGroupBox *parentConfigGroupBox, char *name)
	: SelectFile(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigSelectFile::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigSelectFile::loadConfiguration()
{
	setFile(config_file.readEntry(section, item));
}

void ConfigSelectFile::saveConfiguration()
{
	config_file.writeEntry(section, item, file());
}

bool ConfigSelectFile::fromDomElement(QDomElement domElement)
{
	QString type = domElement.attribute("type");
	if (type.isEmpty())
		return false;

	setType(type);

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigPreview::ConfigPreview(const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: Preview(parentConfigGroupBox->widget(), name), ConfigWidget(widgetCaption, parentConfigGroupBox)
{
	createWidgets();
}

ConfigPreview::ConfigPreview(ConfigGroupBox *parentConfigGroupBox, char *name)
	: Preview(parentConfigGroupBox->widget(), name), ConfigWidget(parentConfigGroupBox)
{
}

void ConfigPreview::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight | Qt::AlignTop);
	layout->addWidget(this, numRows, 1);
}
