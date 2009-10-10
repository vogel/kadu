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

#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLineEdit>

#include "contacts/contact.h"

#include "exports.h"

class ContactAccountData;
class ContactAccountDataManager;

class KADUAPI ContactGeneralConfigurationWidget : public QWidget
{
	Q_OBJECT

	QLineEdit *DisplayEdit;
	QLineEdit *PhoneEdit;
	QLineEdit *MobileEdit;
	QLineEdit *EmailEdit;
	QLineEdit *WebsiteEdit;

	QList<QLineEdit *> ContactsIds;
	QList<QComboBox *> ContactsAccounts;

	QGridLayout *AccountsLayout;
	QGridLayout *ContactsLayout;

	Contact CurrentContact;
	void createGui();

public:
	ContactGeneralConfigurationWidget(Contact &contact, QWidget *parent = 0);
	~ContactGeneralConfigurationWidget();

	void saveConfiguration();
private slots:
	void addAccountDataRow(ContactAccountData *data = 0);

};

#endif // CONTACT_GENERAL_CONFIGURATION_WIDGET
