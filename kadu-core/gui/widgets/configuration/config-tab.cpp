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

ConfigTab::ConfigTab(const QString &name, ConfigSection *configSection, QWidget *mainWidget) :
		MyName(name), MyConfigSection(configSection)
{
	MyScrollArea = new KaduScrollArea(mainWidget);
	MyScrollArea->setFrameStyle(QFrame::NoFrame);
	MyScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	MyScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	MyScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	MyMainWidget = new QWidget(mainWidget);
	MyMainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	MyMainLayout = new QVBoxLayout(MyMainWidget);
	MyMainLayout->addStretch(1);

	MyScrollArea->setWidget(MyMainWidget);
	MyScrollArea->setWidgetResizable(true);
}

ConfigTab::~ConfigTab()
{
	delete MyScrollArea;
}

void ConfigTab::removedConfigGroupBox(const QString &groupBoxName)
{
	MyConfigGroupBoxes.remove(groupBoxName);

	if (!MyConfigGroupBoxes.count())
	{
		MyConfigSection->removedConfigTab(MyName);
		delete this;
	}
}

ConfigGroupBox *ConfigTab::configGroupBox(const QString &name, bool create)
{
	if (MyConfigGroupBoxes.contains(name))
		return MyConfigGroupBoxes[name];

	if (!create)
		return 0;

	QGroupBox *groupBox = new QGroupBox(name, MyMainWidget);
	QHBoxLayout *groupBoxLayout = new QHBoxLayout(groupBox);
	groupBoxLayout->setSizeConstraint(QLayout::SetMinimumSize);

	MyMainLayout->insertWidget(MyConfigGroupBoxes.count(), groupBox);

	ConfigGroupBox *newConfigGroupBox = new ConfigGroupBox(name, this, groupBox);
	MyConfigGroupBoxes[name] = newConfigGroupBox;

	groupBox->show();

	return newConfigGroupBox;
}

QWidget * ConfigTab::scrollWidget()
{
	return MyScrollArea;
}
