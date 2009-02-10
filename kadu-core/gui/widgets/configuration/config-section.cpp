/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QListWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QLayout>

#include "gui/widgets/configuration/config-section.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-tab.h"
#include "gui/widgets/configuration/config-widget.h"

#include "config_file.h"
#include "icons_manager.h"

ConfigSection::ConfigSection(const QString &name, ConfigurationWidget *configurationWidget, QListWidgetItem *listWidgetItem, QWidget *parentConfigGroupBoxWidget,
		const QString &pixmap)
	: name(name), configurationWidget(configurationWidget), pixmap(pixmap), listWidgetItem(listWidgetItem), activated(false)
{
	mainWidget = new QTabWidget(parentConfigGroupBoxWidget);
	parentConfigGroupBoxWidget->layout()->addWidget(mainWidget);
	mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWidget->hide();

	connect(icons_manager, SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));
}

ConfigSection::~ConfigSection()
{
	config_file.writeEntry("General", "ConfigurationWindow_" + configurationWidget->name() + "_" + name,
		mainWidget->label(mainWidget->currentPageIndex()));
	delete mainWidget;
}

ConfigGroupBox * ConfigSection::configGroupBox(const QString &tab, const QString &groupBox, bool create)
{
	ConfigTab *ct = configTab(tab, create);
	if (!ct)
		return 0;

	return ct->configGroupBox(groupBox, create);
}

void ConfigSection::activate()
{
	listWidgetItem->listWidget()->setCurrentItem(listWidgetItem);

	if (activated)
		return;

	QString tab = config_file.readEntry("General", "ConfigurationWindow_" + configurationWidget->name() + "_" + name);
	if (configTabs.contains(tab))
		mainWidget->setCurrentPage(mainWidget->indexOf(configTabs[tab]->tabWidget()));
	activated = true;
}

ConfigTab *ConfigSection::configTab(const QString &name, bool create)
{
	if (configTabs.contains(name))
		return configTabs[name];

	if (!create)
		return 0;

	ConfigTab *newConfigTab = new ConfigTab(name, this, mainWidget);
	configTabs[name] = newConfigTab;

	return newConfigTab;
}

void ConfigSection::removedConfigTab(const QString &configTabName)
{
	mainWidget->removePage(configTabs[configTabName]->widget());

	configTabs.remove(configTabName);
	if (!configTabs.count())
	{
		configurationWidget->removedConfigSection(name);
// 		delete this;
	}
}

void ConfigSection::iconThemeChanged()
{
	QListWidget *listWidget = listWidgetItem->listWidget();
	bool current = listWidgetItem->isSelected();
	delete listWidgetItem;

	listWidgetItem = new QListWidgetItem(icons_manager->loadPixmap(pixmap), name, listWidget);
	if (current)
		listWidget->setCurrentItem(listWidgetItem);
}
