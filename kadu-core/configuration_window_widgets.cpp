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
#include <qtooltip.h>

#include "configuration_window.h"
#include "config_file.h"
#include "debug.h"

#include "configuration_window_widgets.h"

ConfigWidget::ConfigWidget(ConfigGroupBox *parentConfigGroupBox)
	: parentConfigGroupBox(parentConfigGroupBox)
{
}

ConfigWidget::ConfigWidget(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox)
	: parentConfigGroupBox(parentConfigGroupBox), widgetCaption(widgetCaption), toolTip(toolTip)
{
}

bool ConfigWidget::fromDomElement(QDomElement domElement)
{
	widgetCaption = domElement.attribute("caption");

	if (widgetCaption.isEmpty())
		return false;

	toolTip = domElement.attribute("tool-tip");

	createWidgets();
	return true;
}

ConfigWidgetValue::ConfigWidgetValue(ConfigGroupBox *parentConfigGroupBox)
	: ConfigWidget(parentConfigGroupBox)
{
}

ConfigWidgetValue::ConfigWidgetValue(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox)
	: ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox), section(section), item(item)
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

ConfigLineEdit::ConfigLineEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: QLineEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigLineEdit::ConfigLineEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: QLineEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigLineEdit::~ConfigLineEdit()
{
	if (label)
		delete label;
}

void ConfigLineEdit::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);

	if (!toolTip.isEmpty())
	{
		QToolTip::add(this, toolTip);
		QToolTip::add(label, toolTip);
	}
}

void ConfigLineEdit::loadConfiguration()
{
	setText(config_file.readEntry(section, item));
}

void ConfigLineEdit::saveConfiguration()
{
	config_file.writeEntry(section, item, text());
}

void ConfigLineEdit::show()
{
	label->show();
	QLineEdit::show();
}

void ConfigLineEdit::hide()
{
	label->hide();
	QLineEdit::hide();
}

ConfigGGPasswordEdit::ConfigGGPasswordEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: ConfigLineEdit(section, item, widgetCaption, toolTip, parentConfigGroupBox, name)
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

ConfigCheckBox::ConfigCheckBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: QCheckBox(widgetCaption, parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox)
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

	if (!toolTip.isEmpty())
		QToolTip::add(this, toolTip);
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

void ConfigCheckBox::show()
{
	QCheckBox::show();
}

void ConfigCheckBox::hide()
{
	QCheckBox::hide();
}

ConfigSpinBox::ConfigSpinBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		int minValue, int maxValue, int step, ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QSpinBox(minValue, maxValue, step, parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox),
		label(0)
{
}

ConfigSpinBox::ConfigSpinBox(ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QSpinBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigSpinBox::~ConfigSpinBox()
{
	if (label)
		delete label;
}

void ConfigSpinBox::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	label = new QLabel(this, widgetCaption + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);

	if (!toolTip.isEmpty())
	{
		QToolTip::add(this, toolTip);
		QToolTip::add(label, toolTip);
	}
}

void ConfigSpinBox::loadConfiguration()
{
	setValue(config_file.readNumEntry(section, item));
	emit valueChanged(value());
}

void ConfigSpinBox::saveConfiguration()
{
	config_file.writeEntry(section, item, value());
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

ConfigComboBox::ConfigComboBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		const QStringList &itemValues, const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QComboBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigComboBox::ConfigComboBox(ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QComboBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigComboBox::~ConfigComboBox()
{
	if (label)
		delete label;
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

	label = new QLabel(this, widgetCaption + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);

	clear();
	insertStringList(itemCaptions);

	if (!toolTip.isEmpty())
	{
		QToolTip::add(this, toolTip);
		QToolTip::add(label, toolTip);
	}
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

void ConfigComboBox::show()
{
	label->show();
	QComboBox::show();
}

void ConfigComboBox::hide()
{
	label->hide();
	QComboBox::hide();
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

ConfigHotKeyEdit::ConfigHotKeyEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: HotKeyEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigHotKeyEdit::ConfigHotKeyEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: HotKeyEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigHotKeyEdit::~ConfigHotKeyEdit()
{
	if (label)
		delete label;
}

void ConfigHotKeyEdit::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);

	if (!toolTip.isEmpty())
	{
		QToolTip::add(this, toolTip);
		QToolTip::add(label, toolTip);
	}
}

void ConfigHotKeyEdit::loadConfiguration()
{
	setShortCut(config_file.readEntry(section, item));
}

void ConfigHotKeyEdit::saveConfiguration()
{
	config_file.writeEntry(section, item, shortCutString());
}

void ConfigHotKeyEdit::show()
{
	label->show();
	HotKeyEdit::show();
}

void ConfigHotKeyEdit::hide()
{
	label->hide();
	HotKeyEdit::hide();
}

ConfigPathListEdit::ConfigPathListEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: PathListEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigPathListEdit::ConfigPathListEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: PathListEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigPathListEdit::~ConfigPathListEdit()
{
	if (label)
		delete label;
}

void ConfigPathListEdit::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);

	if (!toolTip.isEmpty())
	{
		QToolTip::add(this, toolTip);
		QToolTip::add(label, toolTip);
	}
}

void ConfigPathListEdit::loadConfiguration()
{
	setPathList(QStringList::split(QRegExp("(;|:)"), config_file.readEntry(section, item)));
}

void ConfigPathListEdit::saveConfiguration()
{
	config_file.writeEntry(section, item, pathList().join(":"));
}

void ConfigPathListEdit::show()
{
	label->show();
	PathListEdit::show();
}

void ConfigPathListEdit::hide()
{
	label->hide();
	PathListEdit::hide();
}

ConfigColorButton::ConfigColorButton(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: ColorButton(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigColorButton::ConfigColorButton(ConfigGroupBox *parentConfigGroupBox, char *name)
	: ColorButton(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigColorButton::~ConfigColorButton()
{
	if (label)
		delete label;
}

void ConfigColorButton::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);

	if (!toolTip.isEmpty())
	{
		QToolTip::add(this, toolTip);
		QToolTip::add(label, toolTip);
	}
}

void ConfigColorButton::loadConfiguration()
{
	setColor(config_file.readColorEntry(section, item));
}

void ConfigColorButton::saveConfiguration()
{
	config_file.writeEntry(section, item, color());
}

void ConfigColorButton::show()
{
	label->show();
	ColorButton::show();
}

void ConfigColorButton::hide()
{
	label->hide();
	ColorButton::hide();
}

ConfigSelectFont::ConfigSelectFont(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: SelectFont(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigSelectFont::ConfigSelectFont(ConfigGroupBox *parentConfigGroupBox, char *name)
	: SelectFont(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigSelectFont::~ConfigSelectFont()
{
	if (label)
		delete label;
}

void ConfigSelectFont::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);

	if (!toolTip.isEmpty())
	{
		QToolTip::add(this, toolTip);
		QToolTip::add(label, toolTip);
	}
}

void ConfigSelectFont::loadConfiguration()
{
	setFont(config_file.readFontEntry(section, item));
}

void ConfigSelectFont::saveConfiguration()
{
	config_file.writeEntry(section, item, font());
}

void ConfigSelectFont::show()
{
	label->show();
	SelectFont::show();
}

void ConfigSelectFont::hide()
{
	label->hide();
	SelectFont::hide();
}

ConfigSyntaxEditor::ConfigSyntaxEditor(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: SyntaxEditor(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigSyntaxEditor::ConfigSyntaxEditor(ConfigGroupBox *parentConfigGroupBox, char *name)
	: SyntaxEditor(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigSyntaxEditor::~ConfigSyntaxEditor()
{
	if (label)
		delete label;
}

void ConfigSyntaxEditor::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);

	if (!toolTip.isEmpty())
	{
		QToolTip::add(this, toolTip);
		QToolTip::add(label, toolTip);
	}
}

void ConfigSyntaxEditor::loadConfiguration()
{
	setCurrentSyntax(config_file.readEntry(section, item));
}

void ConfigSyntaxEditor::saveConfiguration()
{
	config_file.writeEntry(section, item, currentSyntax());
}

void ConfigSyntaxEditor::show()
{
	label->show();
	SyntaxEditor::show();
}

void ConfigSyntaxEditor::hide()
{
	label->hide();
	SyntaxEditor::hide();
}

bool ConfigSyntaxEditor::fromDomElement(QDomElement domElement)
{
	QString category = domElement.attribute("category");
	if (category.isEmpty())
		return false;

	setCategory(category);

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigActionButton::ConfigActionButton(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, char *name)
	: QPushButton(parentConfigGroupBox->widget(), name), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox)
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

	if (!toolTip.isEmpty())
		QToolTip::add(this, toolTip);
}

void ConfigActionButton::show()
{
	QPushButton::show();
}

void ConfigActionButton::hide()
{
	QPushButton::hide();
}

ConfigSelectFile::ConfigSelectFile(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		const QString &type, ConfigGroupBox *parentConfigGroupBox, char *name)
	: SelectFile(type, parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigSelectFile::ConfigSelectFile(ConfigGroupBox *parentConfigGroupBox, char *name)
	: SelectFile(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigSelectFile::~ConfigSelectFile()
{
	if (label)
		delete label;
}

void ConfigSelectFile::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);

	if (!toolTip.isEmpty())
	{
		QToolTip::add(this, toolTip);
		QToolTip::add(label, toolTip);
	}
}

void ConfigSelectFile::loadConfiguration()
{
	setFile(config_file.readEntry(section, item));
}

void ConfigSelectFile::saveConfiguration()
{
	config_file.writeEntry(section, item, file());
}

void ConfigSelectFile::show()
{
	label->show();
	SelectFile::show();
}

void ConfigSelectFile::hide()
{
	label->hide();
	SelectFile::hide();
}

bool ConfigSelectFile::fromDomElement(QDomElement domElement)
{
	QString type = domElement.attribute("type");
	if (type.isEmpty())
		return false;

	setType(type);

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigPreview::ConfigPreview(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, char *name)
	: Preview(parentConfigGroupBox->widget(), name), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigPreview::ConfigPreview(ConfigGroupBox *parentConfigGroupBox, char *name)
	: Preview(parentConfigGroupBox->widget(), name), ConfigWidget(parentConfigGroupBox), label(0)
{
}

ConfigPreview::~ConfigPreview()
{
	if (label)
		delete label;
}

void ConfigPreview::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight | Qt::AlignTop);
	layout->addWidget(this, numRows, 1);
}

void ConfigPreview::show()
{
	label->show();
	Preview::show();
}

// TODO: czy Preview dzieczyczy z QToolTip?
void ConfigPreview::hide()
{
	label->hide();
	Preview::QWidget::hide();
}

ConfigSlider::ConfigSlider(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		int minValue, int maxValue, int pageStep, ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QSlider(minValue, maxValue, pageStep, 0, Qt::Horizontal, parentConfigGroupBox->widget(), name),
		ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
}

ConfigSlider::ConfigSlider(ConfigGroupBox *parentConfigGroupBox, char *name)
	: QSlider(Qt::Horizontal, parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
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

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	label = new QLabel(this, widgetCaption + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);

	if (!toolTip.isEmpty())
	{
		QToolTip::add(this, toolTip);
		QToolTip::add(label, toolTip);
	}
}

void ConfigSlider::loadConfiguration()
{
	setValue(config_file.readNumEntry(section, item));
	emit valueChanged(value());
}

void ConfigSlider::saveConfiguration()
{
	config_file.writeEntry(section, item, value());
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

	setMinValue(minValue.toInt(&ok));
	if (!ok)
		return false;

	setMaxValue(maxValue.toInt(&ok));
	if (!ok)
		return false;

	setPageStep(pageStep.toInt(&ok));
	if (!ok)
		return false;

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigLabel::ConfigLabel(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, char *name)
	: QLabel(parentConfigGroupBox->widget(), name), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox)
{
	createWidgets();
}

ConfigLabel::ConfigLabel(ConfigGroupBox *parentConfigGroupBox, char *name)
	: QLabel(parentConfigGroupBox->widget(), name), ConfigWidget(parentConfigGroupBox)
{
}

void ConfigLabel::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	setText(widgetCaption);
	layout->addWidget(this, numRows, 1);

	if (!toolTip.isEmpty())
		QToolTip::add(this, toolTip);
}

void ConfigLabel::show()
{
	QLabel::show();
}

void ConfigLabel::hide()
{
	QLabel::hide();
}

ConfigListBox::ConfigListBox(const QString &widgetCaption, const QString &toolTip,
		const QStringList &itemValues, const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QListBox(parentConfigGroupBox->widget(), name), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigListBox::ConfigListBox(ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QListBox(parentConfigGroupBox->widget(), name), ConfigWidget(parentConfigGroupBox), label(0)
{
}

ConfigListBox::~ConfigListBox()
{
	if (label)
		delete label;
}

void ConfigListBox::setItems(const QStringList &itemValues, const QStringList &itemCaptions)
{
	this->itemValues = itemValues;
	this->itemCaptions = itemCaptions;

	clear();
	insertStringList(itemCaptions);
}

void ConfigListBox::createWidgets()
{
	kdebugf();

	connect(this, SIGNAL(activatd(int index)), this, SLOT(activatedSlot(int index)));

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	label = new QLabel(this, widgetCaption + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);

	clear();
	insertStringList(itemCaptions);

	if (!toolTip.isEmpty())
	{
		QToolTip::add(this, toolTip);
		QToolTip::add(label, toolTip);
	}
}

void ConfigListBox::show()
{
	label->show();
	QListBox::show();
}

void ConfigListBox::hide()
{
	label->hide();
	QListBox::hide();
}

bool ConfigListBox::fromDomElement(QDomElement domElement)
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

	return ConfigWidget::fromDomElement(domElement);
}
