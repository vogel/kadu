/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_file.h"
#include "debug.h"
#include "misc/path-conversion.h"

#include "sql-history-module.h"

extern "C" KADU_EXPORT int sql_history_init(bool firstLoad)
{
	kdebugf();
	sqlHistoryModule = new SqlHistoryModule(firstLoad);
	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void sql_history_close()
{
	kdebugf();
	delete sqlHistoryModule;
	sqlHistoryModule = 0;
	kdebugf2();
}

SqlHistoryModule::SqlHistoryModule(bool firstLoad)
{
	if (firstLoad)
		createDefaultConfiguration();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/sql_history.ui"));
	Storage = new HistorySqlStorage();
	History::instance()->registerStorage(Storage);
}

SqlHistoryModule::~SqlHistoryModule()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/sql_history.ui"));
}

void SqlHistoryModule::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
/*
	portSpinBox = dynamic_cast<QSpinBox *>(widgetById("sql_history/databasehostport"));
	connect(portSpinBox, SIGNAL(valueChanged(int)), this, SLOT(portSpinBoxValueChanged(int)));

	driverComboBox = dynamic_cast<QComboBox *>(mainConfigurationWindow->widgetById("sql_history/dbdriver"));
	connect(driverComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(driverComboBoxValueChanged(int)));
	hostLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("sql_history/databasehost"));
	nameLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("sql_history/databasename"));
	userLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("sql_history/databaseuser"));
	passLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("sql_history/databasepass"));
	prefixLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("sql_history/databaseprefix"));
	if(config_file.readEntry("History", "DatabaseDriver") == "QSQLITE")
	{
		portSpinBox->setEnabled(false);
		hostLineEdit->setEnabled(false); 
		userLineEdit->setEnabled(false); 
		nameLineEdit->setEnabled(false); 
		passLineEdit->setEnabled(false);
		prefixLineEdit->setEnabled(false);
	}
*/
}

void SqlHistoryModule::configurationWindowApplied()
{

}

void SqlHistoryModule::driverComboBoxValueChanged(int index)
{
/*
	if(driverComboBox->itemText(index) == "SQLite")
	{
		portSpinBox->setEnabled(false);
		hostLineEdit->setEnabled(false); 
		userLineEdit->setEnabled(false); 
		nameLineEdit->setEnabled(false); 
		passLineEdit->setEnabled(false);
		prefixLineEdit->setEnabled(false);
	}
	else if(driverComboBox->itemText(index) == "MySQL")
	{
		portSpinBox->setEnabled(true);
		portSpinBox->setValue(3306);
		hostLineEdit->setEnabled(true);
		userLineEdit->setEnabled(true); 
		nameLineEdit->setEnabled(true); 
		passLineEdit->setEnabled(true);
		prefixLineEdit->setEnabled(true);
	}
	else if(driverComboBox->itemText(index) == "PostrgeSQL")
	{
		portSpinBox->setEnabled(true);
		portSpinBox->setValue(5432);
		hostLineEdit->setEnabled(true);
		userLineEdit->setEnabled(true); 
		nameLineEdit->setEnabled(true); 
		passLineEdit->setEnabled(true);
		prefixLineEdit->setEnabled(true);
	}
*/
}

void SqlHistoryModule::portSpinBoxValueChanged(int value)
{
/*
	if (portSpinBox->value() < value)
		portSpinBox->setValue(value);
*/
}

void SqlHistoryModule::configurationUpdated()
{
	kdebugf();
	//initializeDatabase();
	kdebugf2();
}


void SqlHistoryModule::createDefaultConfiguration()
{
	config_file.addVariable("History", "DatabaseDriver", "QSQLITE");
	config_file.addVariable("History", "DatabaseFilePath", ggPath("/history/history.db"));
// 	config_file.addVariable("History", "DatabaseTableNamePrefix", "kadu_");

//TODO: by doda� reszt� warto�ci, je�li jezzcze czego� brak

}

SqlHistoryModule *sqlHistoryModule = 0;