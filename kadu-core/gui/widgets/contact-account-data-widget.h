/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_ACCOUNT_DATA_WIDGET
#define CONTACT_ACCOUNT_DATA_WIDGET

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

#include "gui/widgets/configuration/configuration-widget.h"

#include "exports.h"

class ConfigurationContactDataManager;
class Contact;

class KADUAPI ContactAccountDataWidget : public ConfigurationWidget
{
	Q_OBJECT

	Contact *Data;
	void createGui();

public:
	ContactAccountDataWidget(ConfigurationContactDataManager *dataManager, Contact *contactAccountData, QWidget *parent = 0);
	~ContactAccountDataWidget();

};

#endif // CONTACT_ACCOUNT_DATA_WIDGET
