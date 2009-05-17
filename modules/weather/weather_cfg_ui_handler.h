/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WEATHERCFGUIHANDLER_H
#define WEATHERCFGUIHANDLER_H

#include <QtGui/QTreeWidget>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"

class WeatherCfgUiHandler : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT
			
	public:
		WeatherCfgUiHandler();
		~WeatherCfgUiHandler();
		
	private:
		virtual void mainConfigurationWindowCreated( MainConfigurationWindow* mainCfgWindow );
		virtual void configurationUpdated();
		QTreeWidget* serverList_;
		
	private slots:
		void serverListItemChanged( QTreeWidgetItem* item, int column );
		void upClicked();
		void downClicked();
	
};

#endif // WEATHERCFGUIHANDLER
