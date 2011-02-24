/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QFile>
#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QListWidget>
#include <QtXml/QDomElement>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDialogButtonBox>

#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-widget.h"
#include "gui/widgets/configuration/config-section.h"
#include "gui/widgets/configuration/config-line-edit.h"
#include "gui/widgets/configuration/config-gg-password-edit.h"
#include "gui/widgets/configuration/config-check-box.h"
#include "gui/widgets/configuration/config-spin-box.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-hot-key-edit.h"
#include "gui/widgets/configuration/config-path-list-edit.h"
#include "gui/widgets/configuration/config-color-button.h"
#include "gui/widgets/configuration/config-select-font.h"
#include "gui/widgets/configuration/config-syntax-editor.h"
#include "gui/widgets/configuration/config-action-button.h"
#include "gui/widgets/configuration/config-radio-button.h"
#include "gui/widgets/configuration/config-select-file.h"
#include "gui/widgets/configuration/config-preview.h"
#include "gui/widgets/configuration/config-slider.h"
#include "gui/widgets/configuration/config-label.h"
#include "gui/widgets/configuration/config-list-widget.h"
#include "gui/widgets/configuration/config-line-separator.h"
#include "gui/windows/configuration-window.h"

#include "debug.h"

ConfigurationWidget::ConfigurationWidget(ConfigurationWindowDataManager *dataManager, QWidget *parent) :
		QWidget(parent), CurrentSection(0), DataManager(dataManager)
{
	QHBoxLayout *center_layout = new QHBoxLayout(this);
	center_layout->setMargin(0);
	center_layout->setSpacing(0);

	LeftWidget = new QWidget(this);
	LeftWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	LeftWidget->hide();
	QVBoxLayout *left_layout = new QVBoxLayout(LeftWidget);
	left_layout->setMargin(0);
	left_layout->setSpacing(0);

	ContainerWidget = new QWidget(this);
	new QHBoxLayout(ContainerWidget);

	SectionsListWidget = new QListWidget(LeftWidget);
	SectionsListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	SectionsListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	SectionsListWidget->setIconSize(QSize(32, 32));
	connect(SectionsListWidget, SIGNAL(currentTextChanged(const QString &)), this, SLOT(changeSection(const QString &)));
	left_layout->addWidget(SectionsListWidget);

	center_layout->addWidget(LeftWidget);
	center_layout->addWidget(ContainerWidget);
}

ConfigurationWidget::~ConfigurationWidget()
{
	if (SectionsListWidget->currentItem())
		config_file.writeEntry("General", "ConfigurationWindow_" + Name, SectionsListWidget->currentItem()->text());

	disconnect(SectionsListWidget, SIGNAL(currentTextChanged(const QString &)),
			this, SLOT(changeSection(const QString &)));

	// qDeleteAll() won't work here because of connection to destroyed() signal
	foreach (const ConfigSection *cs, ConfigSections)
	{
		disconnect(cs, SIGNAL(destroyed(QObject *)), this, SLOT(configSectionDestroyed(QObject *)));
		delete cs;
	}
}

void ConfigurationWidget::init()
{
	QString lastSection = config_file.readEntry("General", "ConfigurationWindow_" + Name);
	if (ConfigSections.contains(lastSection))
		ConfigSections[lastSection]->activate();
	else if (SectionsListWidget->count() > 0)
		ConfigSections[SectionsListWidget->item(0)->text()]->activate();
}

QList<ConfigWidget *> ConfigurationWidget::appendUiFile(const QString &fileName, bool load)
{
	QList<ConfigWidget *> widgets = processUiFile(fileName);

	if (load)
		foreach (ConfigWidget *widget, widgets)
			if (widget)
				widget->loadConfiguration();

	return widgets;
}

void ConfigurationWidget::removeUiFile(const QString &fileName)
{
	processUiFile(fileName, false);
}

QList<ConfigWidget *>  ConfigurationWidget::processUiFile(const QString &fileName, bool append)
{
	kdebugf();

	QList<ConfigWidget *> result;
	QFile file(fileName);

	QDomDocument uiFile;
	file.open(QIODevice::ReadOnly);

	if (!uiFile.setContent(&file))
	{
		kdebugf2();
		file.close();
		return result;
	}

	file.close();

	QDomElement kaduConfigurationUi = uiFile.documentElement();
	if (kaduConfigurationUi.tagName() != "configuration-ui")
	{
		kdebugf2();
		return result;
	}

	QDomNodeList children = kaduConfigurationUi.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		result += processUiSectionFromDom(children.item(i), append);

	kdebugf2();
	return result;
}

QList<ConfigWidget *> ConfigurationWidget::processUiSectionFromDom(QDomNode sectionNode, bool append)
{
	kdebugf();

	QList<ConfigWidget *> result;
	if (!sectionNode.isElement())
	{
		kdebugf2();
		return result;
	}

	const QDomElement &sectionElement = sectionNode.toElement();
	if (sectionElement.tagName() != "section")
	{
		kdebugf2();
		return result;
	}

	const QString &iconPath = sectionElement.attribute("icon");

	const QString &sectionName = sectionElement.attribute("name");
	if (sectionName.isEmpty())
	{
		kdebugf2();
		return result;
	}

	configSection(iconPath, qApp->translate("@default", sectionName.toAscii().data()), true);

	const QDomNodeList children = sectionElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		result += processUiTabFromDom(children.item(i), iconPath, sectionName, append);

	kdebugf2();
	return result;
}

QList<ConfigWidget *> ConfigurationWidget::processUiTabFromDom(QDomNode tabNode, const QString &iconName,
	const QString &sectionName, bool append)
{
	Q_UNUSED(iconName)

	kdebugf();

	QList<ConfigWidget *> result;
	if (!tabNode.isElement())
	{
		kdebugf2();
		return result;
	}

	const QDomElement &tabElement = tabNode.toElement();
	if (tabElement.tagName() != "tab")
	{
		kdebugf2();
		return result;
	}

	const QString tabName = tabElement.attribute("name");
	if (tabName.isEmpty())
	{
		kdebugf2();
		return result;
	}

	const QDomNodeList &children = tabElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		result += processUiGroupBoxFromDom(children.item(i), sectionName, tabName, append);

	kdebugf2();
	return result;
}

QList<ConfigWidget *> ConfigurationWidget::processUiGroupBoxFromDom(QDomNode groupBoxNode, const QString &sectionName, const QString &tabName, bool append)
{
	kdebugf();

	QList<ConfigWidget *> result;
	if (!groupBoxNode.isElement())
	{
		kdebugf2();
		return result;
	}

	const QDomElement &groupBoxElement = groupBoxNode.toElement();
	if (groupBoxElement.tagName() != "group-box")
	{
		kdebugf2();
		return result;
	}

	const QString groupBoxName = groupBoxElement.attribute("name");
	if (groupBoxName.isEmpty())
	{
		kdebugf2();
		return result;
	}

	const QString groupBoxId = groupBoxElement.attribute("id");

	ConfigGroupBox *configGroupBoxWidget = configGroupBox(sectionName, tabName, groupBoxName, append);
	if (!configGroupBoxWidget)
	{
		kdebugf2();
		return result;
	}

	if (!groupBoxId.isEmpty())
		Widgets[groupBoxId] = configGroupBoxWidget->widget();

	const QDomNodeList &children = groupBoxElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		if (append)
			result.append(appendUiElementFromDom(children.item(i), configGroupBoxWidget));
		else
			removeUiElementFromDom(children.item(i), configGroupBoxWidget);

	kdebugf2();
	return result;
}

ConfigWidget * ConfigurationWidget::appendUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox)
{
	kdebugf();

	if (!uiElementNode.isElement())
	{
		kdebugf2();
		return 0;
	}

	const QDomElement &uiElement = uiElementNode.toElement();
	const QString &tagName = uiElement.tagName();
	ConfigWidget *widget = 0;

	if (tagName == "line-edit")
		widget = new ConfigLineEdit(configGroupBox, DataManager);
	else if (tagName == "gg-password-edit")
		widget = new ConfigGGPasswordEdit(configGroupBox, DataManager);
	else if (tagName == "check-box")
		widget = new ConfigCheckBox(configGroupBox, DataManager);
	else if (tagName == "radio-button")
		widget = new ConfigRadioButton(configGroupBox, DataManager);
	else if (tagName == "spin-box")
		widget = new ConfigSpinBox(configGroupBox, DataManager);
	else if (tagName == "combo-box")
		widget = new ConfigComboBox(configGroupBox, DataManager);
	else if (tagName == "hot-key-edit")
		widget = new ConfigHotKeyEdit(configGroupBox, DataManager);
	else if (tagName == "path-list-edit")
		widget = new ConfigPathListEdit(configGroupBox, DataManager);
	else if (tagName == "color-button")
		widget = new ConfigColorButton(configGroupBox, DataManager);
	else if (tagName == "select-font")
		widget = new ConfigSelectFont(configGroupBox, DataManager);
	else if (tagName == "syntax-editor")
		widget = new ConfigSyntaxEditor(configGroupBox, DataManager);
	else if (tagName == "action-button")
		widget = new ConfigActionButton(configGroupBox, DataManager);
	else if (tagName == "select-file")
		widget = new ConfigSelectFile(configGroupBox, DataManager);
	else if (tagName == "preview")
		widget = new ConfigPreview(configGroupBox, DataManager);
	else if (tagName == "slider")
		widget = new ConfigSlider(configGroupBox, DataManager);
	else if (tagName == "label")
		widget = new ConfigLabel(configGroupBox, DataManager);
	else if (tagName == "list-box")
		widget = new ConfigListWidget(configGroupBox, DataManager);
	else if (tagName == "line-separator")
		widget = new ConfigLineSeparator(configGroupBox, DataManager);
	else
	{
		kdebugf2();
		return 0;
	}

	if (!widget->fromDomElement(uiElement))
	{
		delete widget;
		kdebugf2();
		return 0;
	}

	QString id = uiElement.attribute("id");
	if (!id.isEmpty())
		Widgets[id] = dynamic_cast<QWidget *>(widget);

	widget->show();

	kdebugf2();
	return widget;
}

void ConfigurationWidget::removeUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox)
{
	kdebugf();

	if (!uiElementNode.isElement())
	{
		kdebugf2();
		return;
	}

	const QDomElement &uiElement = uiElementNode.toElement();
	const QString &caption = uiElement.attribute("caption");

	foreach (QObject *child, configGroupBox->widget()->children())
	{
		ConfigWidget *configWidget = dynamic_cast<ConfigWidget *>(child);
		if (!configWidget)
			continue;

		if (configWidget->widgetCaption == caption)
		{
			delete configWidget;
			break;
		}
	}

	if (configGroupBox->empty())
		delete configGroupBox;

	kdebugf2();
}

QWidget * ConfigurationWidget::widgetById(const QString &id)
{
	if (Widgets.contains(id))
		return Widgets[id];

	return 0;
}

ConfigGroupBox * ConfigurationWidget::configGroupBox(const QString &section, const QString &tab, const QString &groupBox, bool create)
{
	ConfigSection *s = configSection(qApp->translate("@default", section.toAscii().data()));
	if (!s)
		return 0;

	return s->configGroupBox(qApp->translate("@default", tab.toAscii().data()), qApp->translate("@default", groupBox.toAscii().data()), create);
}

ConfigSection * ConfigurationWidget::configSection(const QString &name)
{
	return ConfigSections[name];
}

ConfigSection * ConfigurationWidget::configSection(const QString &iconPath, const QString &name, bool create)
{
	if (ConfigSections.contains(name))
		return ConfigSections[name];

	if (!create)
		return 0;

	QListWidgetItem *newConfigSectionListWidgetItem = new QListWidgetItem(IconsManager::instance()->iconByPath(iconPath).pixmap(32, 32), name, SectionsListWidget);

	QFontMetrics fontMetrics = SectionsListWidget->fontMetrics();
	// TODO: 48 = margins + scrollbar - get real scrollbar width
	int width = fontMetrics.width(name) + 80;

	ConfigSection *newConfigSection = new ConfigSection(name, this, newConfigSectionListWidgetItem, ContainerWidget, iconPath);
	ConfigSections[name] = newConfigSection;
	connect(newConfigSection, SIGNAL(destroyed(QObject *)), this, SLOT(configSectionDestroyed(QObject *)));

	if (ConfigSections.count() == 1)
		SectionsListWidget->setFixedWidth(width);

	if (ConfigSections.count() > 1)
	{
		if (SectionsListWidget->width() < width)
			SectionsListWidget->setFixedWidth(width);
		LeftWidget->show();
	}

	return newConfigSection;
}

void ConfigurationWidget::loadConfiguration(QObject *object)
{
	kdebugf();

	if (!object)
		return;

	const QObjectList children = object->children();
	foreach (QObject *child, children)
		loadConfiguration(child);

	ConfigWidget *configWidget = dynamic_cast<ConfigWidget *>(object);
	if (configWidget)
		configWidget->loadConfiguration();
}

void ConfigurationWidget::loadConfiguration()
{
	loadConfiguration(this);
}

void ConfigurationWidget::saveConfiguration(QObject *object)
{
	kdebugf();

	if (!object)
		return;

	const QObjectList children = object->children();
	foreach (QObject *child, children)
		saveConfiguration(child);

	ConfigWidget *configWidget = dynamic_cast<ConfigWidget *>(object);
	if (configWidget)
		configWidget->saveConfiguration();
}

void ConfigurationWidget::saveConfiguration()
{
	saveConfiguration(this);
}

void ConfigurationWidget::changeSection(const QString &newSectionName)
{
	if (!ConfigSections.contains(newSectionName))
		return;

	ConfigSection *newSection = ConfigSections[newSectionName];
	if (newSection == CurrentSection)
		return;

	if (CurrentSection)
		CurrentSection->hide();

	CurrentSection = newSection;
	newSection->show();
	newSection->activate();
}

void ConfigurationWidget::configSectionDestroyed(QObject *obj)
{
	// see ConfigSection::~ConfigSection()
	disconnect(obj, SIGNAL(destroyed(QObject *)), this, SLOT(configGroupBoxDestroyed(QObject *)));

	ConfigSections.remove(static_cast<ConfigSection *>(obj)->name());

	if (CurrentSection == obj)
		CurrentSection = 0;
}
