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
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeView>

#include "buddies/buddy.h"

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

	QDialog *OrderDialog; 

	QList<QLineEdit *> ContactsIds;
	QList<QComboBox *> ContactsAccounts;

	QGridLayout *AccountsLayout;
	QGridLayout *ContactsLayout;

	Buddy CurrentContact;
	void createGui();
	void updateOrder();

public:
	ContactGeneralConfigurationWidget(Buddy &contact, QWidget *parent = 0);
	~ContactGeneralConfigurationWidget();

	void saveConfiguration();
private slots:
	void addAccountDataRow(ContactAccountData *data = 0);
	void showOrderDialog();
	void updateOrderAndClose();

};

#endif // CONTACT_GENERAL_CONFIGURATION_WIDGET
