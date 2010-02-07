/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
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
#include <QtGui/QListWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>

#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/config-section.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-tab.h"
#include "gui/widgets/configuration/config-widget.h"

#include "icons-manager.h"

ConfigSection::ConfigSection(const QString &name, ConfigurationWidget *configurationWidget, QListWidgetItem *listWidgetItem, QWidget *parentConfigGroupBoxWidget,
		const QString &pixmap) :
		name(name), configurationWidget(configurationWidget), pixmap(pixmap), listWidgetItem(listWidgetItem), activated(false), parentConfigGroupBoxWidget(parentConfigGroupBoxWidget)
{
	mainWidget = new QWidget(parentConfigGroupBoxWidget);
	new QHBoxLayout(mainWidget);
	mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	tabWidget = 0;

	parentConfigGroupBoxWidget->layout()->addWidget(mainWidget);
	mainWidget->hide();

	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));
}

ConfigSection::~ConfigSection()
{
	if (tabWidget)
		config_file.writeEntry("General", "ConfigurationWindow_" + configurationWidget->name() + "_" + name,
							   tabWidget->tabText(tabWidget->currentIndex()));
	delete mainWidget;
}

void ConfigSection::switchTabView(bool tabView)
{
	if (tabView == (0 != tabWidget))
		return;

	bool isVisible = mainWidget->isVisible();

	if (tabView)
	{
		tabWidget = new QTabWidget(parentConfigGroupBoxWidget);
		parentConfigGroupBoxWidget->layout()->addWidget(tabWidget);
		tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		mainWidget->hide();
		foreach (ConfigTab *configTab, configTabs)
			tabWidget->addTab(configTab->tabWidget(), configTab->name());

		delete mainWidget;
		mainWidget = tabWidget;
	}
	else
	{
		mainWidget = new QWidget(parentConfigGroupBoxWidget);
		new QHBoxLayout(mainWidget);
		parentConfigGroupBoxWidget->layout()->addWidget(mainWidget);
		mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		foreach (ConfigTab *configTab, configTabs)
			configTab->tabWidget()->setParent(mainWidget);
	
		delete tabWidget;
		tabWidget = 0;
	}

	mainWidget->setVisible(isVisible);
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
	if (tabWidget && configTabs.contains(tab))
		tabWidget->setCurrentWidget(configTabs[tab]->tabWidget());
	activated = true;
}

ConfigTab * ConfigSection::configTab(const QString &name, bool create)
{
	if (configTabs.contains(name))
		return configTabs[name];

	if (!create)
		return 0;

	switchTabView(configTabs.size() >= 1);

	ConfigTab *newConfigTab = new ConfigTab(name, this, mainWidget);
	configTabs[name] = newConfigTab;

	if (tabWidget)
		tabWidget->addTab(newConfigTab->scrollWidget(), newConfigTab->name());
	else
		mainWidget->layout()->addWidget(newConfigTab->scrollWidget());

	return newConfigTab;
}

void ConfigSection::removedConfigTab(const QString &configTabName)
{
	if (tabWidget)
	{
		tabWidget->removeTab(tabWidget->indexOf(configTabs[configTabName]->widget()));
	}

	configTabs.remove(configTabName);
	if (!configTabs.count())
	{
		configurationWidget->removedConfigSection(name);
// 		delete this;
	}

	switchTabView(configTabs.size() > 1);
}

void ConfigSection::iconThemeChanged()
{
	QListWidget *listWidget = listWidgetItem->listWidget();
	bool current = listWidgetItem->isSelected();
	delete listWidgetItem;

	listWidgetItem = new QListWidgetItem(IconsManager::instance()->pixmapByName(pixmap), name, listWidget);
	if (current)
		listWidget->setCurrentItem(listWidgetItem);
}
