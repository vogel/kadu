/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_PERSONAL_INFO_CONFIGURATION_WIDGET
#define CONTACT_PERSONAL_INFO_CONFIGURATION_WIDGET

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

#include "contacts/contact.h"
#include "gui/widgets/configuration/configuration-widget.h"

#include "exports.h"

class ContactAccountData;
class ContactAccountDataManager;

class KADUAPI ContactPersonalInfoConfigurationWidget : public QWidget
{
	Q_OBJECT

	Contact CurrentContact;
	void createGui();

public:
	ContactPersonalInfoConfigurationWidget(Contact &contact, QWidget *parent = 0);
	~ContactPersonalInfoConfigurationWidget();

};

#endif // CONTACT_PERSONAL_INFO_CONFIGURATION_WIDGET
