/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_GENERAL_CONFIGURATION_WIDGET
#define CONTACT_GENERAL_CONFIGURATION_WIDGET

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

#include "gui/widgets/configuration/configuration-widget.h"

#include "exports.h"

class ContactAccountData;
class ContactAccountDataManager;

class KADUAPI ContactGeneralConfigurationWidget : public QWidget
{
	Q_OBJECT

	void createGui();

public:
	ContactGeneralConfigurationWidget(QWidget *parent = 0);
	~ContactGeneralConfigurationWidget();

};

#endif // CONTACT_GENERAL_CONFIGURATION_WIDGET
