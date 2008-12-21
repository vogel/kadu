/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QScrollArea>
#include <QtGui/QTabWidget>

#include "gui/widgets/configuration/config-tab.h"
#include "gui/widgets/configuration/config-section.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/kadu-scroll-area.h"

ConfigTab::ConfigTab(const QString &name, ConfigSection *configSection, QTabWidget *tabWidget)
	: name(name), configSection(configSection)
{
	scrollArea = new KaduScrollArea(tabWidget);
	scrollArea->setFrameStyle(QFrame::NoFrame);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

	mainWidget = new QWidget(tabWidget);
	mainLayout = new QVBoxLayout(mainWidget);
	mainLayout->addStretch(1);

	tabWidget->addTab(scrollArea, name);
	scrollArea->setWidget(mainWidget);
	scrollArea->setWidgetResizable(true);
}

ConfigTab::~ConfigTab()
{
	delete scrollArea;
}

void ConfigTab::removedConfigGroupBox(const QString &groupBoxName)
{
	configGroupBoxes.remove(groupBoxName);

	if (!configGroupBoxes.count())
	{
		configSection->removedConfigTab(name);
		delete this;
	}
}

ConfigGroupBox *ConfigTab::configGroupBox(const QString &name, bool create)
{
	if (configGroupBoxes.contains(name))
		return configGroupBoxes[name];

	if (!create)
		return 0;

	QGroupBox *groupBox = new QGroupBox(name, mainWidget);
	QHBoxLayout *groupBoxLayout = new QHBoxLayout(groupBox);
	groupBoxLayout->setSizeConstraint(QLayout::SetMinimumSize);

	mainLayout->insertWidget(configGroupBoxes.count(), groupBox);

	ConfigGroupBox *newConfigGroupBox = new ConfigGroupBox(name, this, groupBox);
	configGroupBoxes[name] = newConfigGroupBox;

	groupBox->show();

	return newConfigGroupBox;
}
