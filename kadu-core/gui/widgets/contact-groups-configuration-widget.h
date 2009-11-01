/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_GROUPS_CONFIGURATION_WIDGET
#define CONTACT_GROUPS_CONFIGURATION_WIDGET

#include <QtGui/QCheckBox>
#include <QtGui/QScrollArea>

#include "buddies/buddy.h"

#include "exports.h"

class KADUAPI ContactGroupsConfigurationWidget : public QScrollArea
{
	Q_OBJECT
  
	Buddy CurrentContact;
	QList<QCheckBox *> GroupCheckBoxList;
	void createGui();

public:
	ContactGroupsConfigurationWidget(Buddy &contact, QWidget *parent = 0);
	~ContactGroupsConfigurationWidget();
	void saveConfiguration();

};

#endif // CONTACT_GROUPS_CONFIGURATION_WIDGET
