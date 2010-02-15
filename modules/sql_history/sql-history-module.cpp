/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include <QtGui/QPushButton>

#include "configuration/configuration-file.h"
#include "debug.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/path-conversion.h"

#include "modules/history/history.h"

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
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/sql-history.ui"));
	MainConfigurationWindow::registerUiHandler(this);
	Storage = new HistorySqlStorage();
	History::instance()->registerStorage(Storage);
}

SqlHistoryModule::~SqlHistoryModule()
{
	MainConfigurationWindow::unregisterUiHandler(this);
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/sql-history.ui"));
}

void SqlHistoryModule::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	//TODO 0.6.6
	QPushButton *clickMe = dynamic_cast<QPushButton *>(mainConfigurationWindow->widget()->widgetById("sql_history/import"));
	connect(clickMe, SIGNAL(clicked(bool)), this, SLOT(doSomeImport()));

	portSpinBox = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("sql_history/databasehostport"));
	connect(portSpinBox, SIGNAL(valueChanged(int)), this, SLOT(portSpinBoxValueChanged(int)));

	driverComboBox = dynamic_cast<QComboBox *>(mainConfigurationWindow->widget()->widgetById("sql_history/dbdriver"));
	connect(driverComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(driverComboBoxValueChanged(int)));
	hostLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("sql_history/databasehost"));
	nameLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("sql_history/databasename"));
	userLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("sql_history/databaseuser"));
	passLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("sql_history/databasepass"));
	prefixLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("sql_history/databaseprefix"));
	if(config_file.readEntry("History", "DatabaseDriver") == "QSQLITE")
	{
		portSpinBox->setEnabled(false);
		hostLineEdit->setEnabled(false); 
		userLineEdit->setEnabled(false); 
		nameLineEdit->setEnabled(false); 
		passLineEdit->setEnabled(false);
		prefixLineEdit->setEnabled(false);
	}

}

void SqlHistoryModule::configurationWindowApplied()
{

}

void SqlHistoryModule::driverComboBoxValueChanged(int index)
{

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

}

void SqlHistoryModule::portSpinBoxValueChanged(int value)
{
	if (portSpinBox->value() < value)
		portSpinBox->setValue(value);
}

void SqlHistoryModule::configurationUpdated()
{
	kdebugf();
	//Storage->initializeDatabase();
	kdebugf2();
}

//TODO 0.6.6
void SqlHistoryModule::doSomeImport()
{
	kdebugf();
	
	Storage->convertSenderToContact();
	
	kdebugf2();
}

void SqlHistoryModule::createDefaultConfiguration()
{
	config_file.addVariable("History", "DatabaseDriver", "QSQLITE");
	config_file.addVariable("History", "DatabaseFilePath", profilePath("/history/history.db"));
	config_file.addVariable("History", "DatabaseTableNamePrefix", "kadu_");
}

SqlHistoryModule *sqlHistoryModule = 0;
