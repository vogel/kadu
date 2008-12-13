/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>

#include "configuration/configuration-window.h"
#include "debug.h"
#include "misc.h"

#include "configuration-window-widgets.h"

ConfigWidget::ConfigWidget(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: parentConfigGroupBox(parentConfigGroupBox), dataManager(dataManager)
{
}

ConfigWidget::ConfigWidget(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: parentConfigGroupBox(parentConfigGroupBox), widgetCaption(widgetCaption), toolTip(toolTip), dataManager(dataManager)
{
}

bool ConfigWidget::fromDomElement(QDomElement domElement)
{
	widgetCaption = domElement.attribute("caption");

	if (widgetCaption.isEmpty())
		return false;

	ConfigWidget::toolTip = domElement.attribute("tool-tip");

	createWidgets();
	return true;
}

ConfigWidgetValue::ConfigWidgetValue(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: ConfigWidget(parentConfigGroupBox, dataManager)
{
}

ConfigWidgetValue::ConfigWidgetValue(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox, dataManager), section(section), item(item)
{
}

bool ConfigWidgetValue::fromDomElement(QDomElement domElement)
{
	section = domElement.attribute("config-section");
	item = domElement.attribute("config-item");

	return ConfigWidget::fromDomElement(domElement);
}

ConfigLineEdit::ConfigLineEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: QLineEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigLineEdit::ConfigLineEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: QLineEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
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

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigLineEdit::loadConfiguration()
{
	setText(dataManager->readEntry(section, item).toString());
}

void ConfigLineEdit::saveConfiguration()
{
	dataManager->writeEntry(section, item, QVariant(text()));
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

ConfigGGPasswordEdit::ConfigGGPasswordEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: ConfigLineEdit(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager, name)
{
	setEchoMode(QLineEdit::Password);
}

ConfigGGPasswordEdit::ConfigGGPasswordEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: ConfigLineEdit(parentConfigGroupBox, dataManager, name)
{
	setEchoMode(QLineEdit::Password);
}

void ConfigGGPasswordEdit::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setText(pwHash(dataManager->readEntry(section, item).toString()));
}

void ConfigGGPasswordEdit::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, QVariant(pwHash(text())));
}

ConfigCheckBox::ConfigCheckBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: QCheckBox(widgetCaption, parentConfigGroupBox->widget(), name), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager)
{
	createWidgets();
}

ConfigCheckBox::ConfigCheckBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: QCheckBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, dataManager)
{
}

void ConfigCheckBox::createWidgets()
{
	kdebugf();

	setText(qApp->translate("@default", widgetCaption));
	parentConfigGroupBox->addWidget(this, true);

	if (!ConfigWidget::toolTip.isEmpty())
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
}

void ConfigCheckBox::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setChecked(dataManager->readEntry(section, item).toBool());
	emit toggled(isChecked());
}

void ConfigCheckBox::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, QVariant(isChecked() ? "true" : "false"));
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
		int minValue, int maxValue, int step, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name)
	: QSpinBox(minValue, maxValue, step, parentConfigGroupBox->widget(), name), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager),
		label(0)
{
}

ConfigSpinBox::ConfigSpinBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name)
	: QSpinBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
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

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigSpinBox::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setValue(dataManager->readEntry(section, item).toInt());
	emit valueChanged(value());
}

void ConfigSpinBox::saveConfiguration()
{
	if (section.isEmpty())
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
		setLineStep(1);

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigComboBox::ConfigComboBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		const QStringList &itemValues, const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name)
	: QComboBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigComboBox::ConfigComboBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name)
	: QComboBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
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

QString ConfigComboBox::currentItemValue()
{
	int index = currentItem();

	if ((index < 0) || (index >= itemValues.size()))
		return QString::null;

	return itemValues[index];
}

void ConfigComboBox::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	clear();
	insertStringList(itemCaptions);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigComboBox::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setCurrentItem(itemValues.findIndex(dataManager->readEntry(section, item).toString()));

	emit activated(currentItem());
}

void ConfigComboBox::saveConfiguration()
{
	if (section.isEmpty())
		return;

	int index = currentItem();

	if ((index < 0) || (index >= itemValues.size()))
		return;

	dataManager->writeEntry(section, item, QVariant(itemValues[currentItem()]));
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
			itemCaptions.append(qApp->translate("@default", element.attribute("caption")));

			insertItem(qApp->translate("@default", element.attribute("caption")));
		}
	}

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigHotKeyEdit::ConfigHotKeyEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: HotKeyEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigHotKeyEdit::ConfigHotKeyEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: HotKeyEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
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

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigHotKeyEdit::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setShortCut(dataManager->readEntry(section, item).toString());
}

void ConfigHotKeyEdit::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, QVariant(shortCutString()));
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
		ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: PathListEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigPathListEdit::ConfigPathListEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: PathListEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
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

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigPathListEdit::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setPathList(QStringList::split(QRegExp("&"), dataManager->readEntry(section, item).toString()));
}

void ConfigPathListEdit::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, QVariant(pathList().join("&")));
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

ConfigColorButton::ConfigColorButton(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: ColorButton(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigColorButton::ConfigColorButton(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: ColorButton(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
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

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigColorButton::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setColor(dataManager->readEntry(section, item).value<QColor>());
}

void ConfigColorButton::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, QVariant(color().name()));
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
		ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: SelectFont(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigSelectFont::ConfigSelectFont(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: SelectFont(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
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

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigSelectFont::loadConfiguration()
{
	if (section.isEmpty())
		return;
	QFont font;
	font.fromString(dataManager->readEntry(section, item).toString());
	setFont(font);
}

void ConfigSelectFont::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, QVariant(font().toString()));
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
		ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: SyntaxEditor(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigSyntaxEditor::ConfigSyntaxEditor(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: SyntaxEditor(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
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

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigSyntaxEditor::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setCurrentSyntax(dataManager->readEntry(section, item).toString());
}

void ConfigSyntaxEditor::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, currentSyntax());
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
	QString syntaxHint = domElement.attribute("syntax-hint");
	if (category.isEmpty())
		return false;

	setCategory(category);
	setSyntaxHint(syntaxHint);

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigActionButton::ConfigActionButton(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: QPushButton(parentConfigGroupBox->widget(), name), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox, dataManager)
{
	createWidgets();
}

ConfigActionButton::ConfigActionButton(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: QPushButton(parentConfigGroupBox->widget(), name), ConfigWidget(parentConfigGroupBox, dataManager)
{
}

void ConfigActionButton::createWidgets()
{
	kdebugf();

	setText(qApp->translate("@default", widgetCaption));
	parentConfigGroupBox->addWidget(this);

	if (!ConfigWidget::toolTip.isEmpty())
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
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
		const QString &type, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: SelectFile(type, parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigSelectFile::ConfigSelectFile(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: SelectFile(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
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

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigSelectFile::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setFile(dataManager->readEntry(section, item).toString());
}

void ConfigSelectFile::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, file());
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

ConfigPreview::ConfigPreview(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: Preview(parentConfigGroupBox->widget()), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigPreview::ConfigPreview(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: Preview(parentConfigGroupBox->widget()), ConfigWidget(parentConfigGroupBox, dataManager), label(0)
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

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);
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
		int minValue, int maxValue, int pageStep, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name)
	: QSlider(minValue, maxValue, pageStep, 0, Qt::Horizontal, parentConfigGroupBox->widget(), name),
		ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
}

ConfigSlider::ConfigSlider(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: QSlider(Qt::Horizontal, parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
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

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
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

ConfigLabel::ConfigLabel(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: QLabel(parentConfigGroupBox->widget(), name), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox, dataManager)
{
	createWidgets();
}

ConfigLabel::ConfigLabel(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: QLabel(parentConfigGroupBox->widget(), name), ConfigWidget(parentConfigGroupBox, dataManager)
{
}

void ConfigLabel::createWidgets()
{
	kdebugf();

	setText(qApp->translate("@default", widgetCaption));
	parentConfigGroupBox->addWidget(this);

	if (!ConfigWidget::toolTip.isEmpty())
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
}

void ConfigLabel::show()
{
	QLabel::show();
}

void ConfigLabel::hide()
{
	QLabel::hide();
}

ConfigListWidget::ConfigListWidget(const QString &widgetCaption, const QString &toolTip,
		const QStringList &itemValues, const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name)
	: QListWidget(parentConfigGroupBox->widget()), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigListWidget::ConfigListWidget(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name)
	: QListWidget(parentConfigGroupBox->widget()), ConfigWidget(parentConfigGroupBox, dataManager), label(0)
{
}

ConfigListWidget::~ConfigListWidget()
{
	if (label)
		delete label;
}

void ConfigListWidget::setItems(const QStringList &itemValues, const QStringList &itemCaptions)
{
	this->itemValues = itemValues;
	this->itemCaptions = itemCaptions;

	clear();
	addItems(itemCaptions);
}

void ConfigListWidget::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	clear();
	addItems(itemCaptions);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigListWidget::show()
{
	label->show();
	QListWidget::show();
}

void ConfigListWidget::hide()
{
	label->hide();
	QListWidget::hide();
}

bool ConfigListWidget::fromDomElement(QDomElement domElement)
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

			addItem(qApp->translate("@default", element.attribute("caption")));
		}
	}

	return ConfigWidget::fromDomElement(domElement);
}

ConfigManageAccounts::ConfigManageAccounts(const QString &widgetCaption,
		const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name)
	: ManageAccounts(parentConfigGroupBox->widget()), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox, dataManager)
{
}

ConfigManageAccounts::ConfigManageAccounts(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: ManageAccounts(parentConfigGroupBox->widget()), ConfigWidget(parentConfigGroupBox, dataManager)
{
}

ConfigManageAccounts::~ConfigManageAccounts()
{
}

void ConfigManageAccounts::createWidgets()
{
	if (!ConfigWidget::toolTip.isEmpty())
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip));

	parentConfigGroupBox->addWidget(this, true);
}

void ConfigManageAccounts::loadConfiguration()
{
}

void ConfigManageAccounts::saveConfiguration()
{
}

void ConfigManageAccounts::show()
{
	ManageAccounts::show();
}

void ConfigManageAccounts::hide()
{
	ManageAccounts::hide();
}
