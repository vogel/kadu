/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQL_HISTORY_MODULE
#define SQL_HISTORY_MODULE

#include <QtCore/QObject>
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QSpinBox>

#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"

#include "storage/history-sql-storage.h"

class SqlHistoryModule : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	QSpinBox *portSpinBox;	
	QComboBox *driverComboBox;
	QLineEdit* hostLineEdit;
	QLineEdit* userLineEdit; 
	QLineEdit* nameLineEdit;  
	QLineEdit* passLineEdit;
	QLineEdit* prefixLineEdit;

	HistorySqlStorage *Storage;

	virtual void configurationUpdated();
	void createDefaultConfiguration(); 

private slots:
	void driverComboBoxValueChanged(int index);
	void configurationWindowApplied();
	void portSpinBoxValueChanged(int value);

public:
	SqlHistoryModule(bool firstLoad);
	~SqlHistoryModule();
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);


};

extern SqlHistoryModule *sqlHistoryModule;

#endif //  SQL_HISTORY_MODULE
