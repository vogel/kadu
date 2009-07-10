 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_PERSONAL_INFO_WIDGET_H
#define GADU_PERSONAL_INFO_WIDGET_H

#include <QtGui/QWidget>

#include "contacts/contact.h"

class QComboBox;
class QLineEdit;

class Account;
class PersonalInfoService;

class GaduPersonalInfoWidget : public QWidget
{
	Q_OBJECT

	PersonalInfoService *Service;

	QLineEdit *NickName;
	QLineEdit *FirstName;
	QLineEdit *LastName;
	QComboBox *Sex;
	QLineEdit *FamilyName;
	QLineEdit *BirthYear;
	QLineEdit *City;
	QLineEdit *FamilyCity;

	void createGui();

private slots:
	void personalInfoAvailable(Contact contact);

public:
	explicit GaduPersonalInfoWidget(Account *account, QWidget *parent = 0);
	virtual ~GaduPersonalInfoWidget();

	void applyData();

};

#endif // GADU_PERSONAL_INFO_WIDGET_H
