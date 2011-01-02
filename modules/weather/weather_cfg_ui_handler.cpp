/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QTreeWidget>

#include "config_file.h"
#include "icons_manager.h"

#include "weather_cfg_ui_handler.h"
#include "weather_global.h"

WeatherCfgUiHandler::WeatherCfgUiHandler()
{
	// Load configuration UI
	//
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/weather.ui"), this);
}

WeatherCfgUiHandler::~WeatherCfgUiHandler()
{
	// Unload configuration UI
	//
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/weather.ui"), this);
}

void WeatherCfgUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainCfgWindow)
{
	ConfigGroupBox *weatherServersBox = mainCfgWindow->configGroupBox("Weather", "General", "Weather servers", false);
	weatherServersBox->addWidget(new QLabel(tr("Servers priorites:"), weatherServersBox->widget()), true);
	QWidget *serversBox = new QWidget(weatherServersBox->widget());
	QHBoxLayout *serversBoxLayout = new QHBoxLayout(serversBox);
	serversBoxLayout->setSpacing(5);
	QTreeWidgetItem *headerItem = new QTreeWidgetItem();
	headerItem->setText(0, tr("Server name"));
	serverList_ = new QTreeWidget(serversBox);
	serverList_->setColumnCount(1);
	serverList_->setHeaderItem(headerItem);
	serverList_->setAllColumnsShowFocus(true);
	serverList_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	serverList_->setMaximumHeight(70);
	serverList_->setHeaderHidden(true);
	QVBoxLayout *buttonsBox = new QVBoxLayout();
	buttonsBox->setSpacing(5);
	QPushButton *upButton = new QPushButton(tr("Up"), serversBox, "weather/up");
	QPushButton *downButton = new QPushButton(tr("Down"), serversBox, "weather/down");
	buttonsBox->addWidget(upButton);
	buttonsBox->addWidget(downButton);
	serversBoxLayout->addWidget(serverList_);
	serversBoxLayout->addLayout(buttonsBox);
	weatherServersBox->addWidget(serversBox, true);

	connect(upButton, SIGNAL(clicked()), this, SLOT(upClicked()));
	connect(downButton, SIGNAL(clicked()), this, SLOT(downClicked()));
	connect(mainCfgWindow->widgetById("weather/autodownload"), SIGNAL(toggled(bool)),
			 mainCfgWindow->widgetById("weather/hint_box"), SLOT(setEnabled(bool)));
	connect(mainCfgWindow->widgetById("weather/autodownload"), SIGNAL(toggled(bool)),
			 mainCfgWindow->widgetById("weather/desc_box"), SLOT(setEnabled(bool)));
	connect(mainCfgWindow->widgetById("weather/showhint"), SIGNAL(toggled(bool)),
			 mainCfgWindow->widgetById("weather/hint_text"), SLOT(setEnabled(bool)));
	connect(mainCfgWindow->widgetById("weather/showhint"), SIGNAL(toggled(bool)),
			 mainCfgWindow->widgetById("weather/hint_day"), SLOT(setEnabled(bool)));
	connect(mainCfgWindow->widgetById("weather/updatedescription"), SIGNAL(toggled(bool)),
			 mainCfgWindow->widgetById("weather/desc_text"), SLOT(setEnabled(bool)));
	connect(mainCfgWindow->widgetById("weather/updatedescription"), SIGNAL(toggled(bool)),
			 mainCfgWindow->widgetById("weather/desc_pos"), SLOT(setEnabled(bool)));
	connect(mainCfgWindow->widgetById("weather/updatedescription"), SIGNAL(toggled(bool)),
			 mainCfgWindow->widgetById("weather/desc_day"), SLOT(setEnabled(bool)));
	connect(serverList_, SIGNAL(itemPressed(QTreeWidgetItem *, int)), this, SLOT(serverListItemChanged(QTreeWidgetItem *, int)));

	// Weather servers
	//
	WeatherGlobal::SERVERITERATOR server;
	for (server = weather_global->servers_.begin(); server != weather_global->servers_.end(); ++server)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(serverList_);
		item->setText(0, server->name_);
		item->setCheckState(0, server->use_ ? Qt::Checked : Qt::Unchecked);
	}
}

void WeatherCfgUiHandler::configurationUpdated()
{
	if (!MainConfigurationWindow::instance()->isShown()) return;

	for (int i = 0; i < serverList_->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *item = serverList_->topLevelItem(i);
		QString itemText = item->text(0);
		weather_global->setServerUsing(itemText, item->checkState(0) == Qt::Checked);
		weather_global->setServerPos(itemText, i);
	}
}


void WeatherCfgUiHandler::serverListItemChanged(QTreeWidgetItem *item, int column)
{
// 	if( item == 0 )
// 		return;
//
// 	if( item->checkState() == Qt::Checked )
// 		return;
//
// 	QCheckListItem* listItem = reinterpret_cast<QCheckListItem*>( serverList_->firstChild() );
//
// 	while( listItem != 0 )
// 	{
// 		if( listItem->isOn() )
// 			return;
// 		listItem = reinterpret_cast<QCheckListItem*>( listItem->nextSibling() );
// 	}
// 	checkItem->setOn( true );
}

void WeatherCfgUiHandler::upClicked()
{
	int index = serverList_->currentIndex().row();
	if (index > 0)
	{
		QTreeWidgetItem *item = serverList_->takeTopLevelItem(index);
		serverList_->insertTopLevelItem(index - 1, item);
		serverList_->setCurrentItem(item);
	}
}

void WeatherCfgUiHandler::downClicked()
{
	int index = serverList_->currentIndex().row();
	if (index < serverList_->topLevelItemCount() - 1)
	{
		QTreeWidgetItem *item = serverList_->takeTopLevelItem(index);
		serverList_->insertTopLevelItem(index + 1, item);
		serverList_->setCurrentItem(item);
	}
}
