/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_WIDGET_H
#define CONTACT_WIDGET_H

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

#include "contacts/contact.h"
#include "gui/widgets/configuration/configuration-widget.h"

#include "exports.h"

class ConfigurationContactDataManager;

class KADUAPI ContactWidget : public ConfigurationWidget
{
	Q_OBJECT

	Contact Data;
	void createGui();

public:
	explicit ContactWidget(ConfigurationContactDataManager *dataManager, Contact contact, QWidget *parent = 0);
	virtual ~ContactWidget();

};

#endif // CONTACT_WIDGET_H
