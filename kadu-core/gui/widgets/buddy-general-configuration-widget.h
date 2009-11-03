/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_GENERAL_CONFIGURATION_WIDGET_H
#define BUDDY_GENERAL_CONFIGURATION_WIDGET_H

#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeView>

#include "buddies/buddy.h"

#include "exports.h"

class ContactAccountData;
class ContactAccountDataManager;

class KADUAPI BuddyGeneralConfigurationWidget : public QWidget
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

	Buddy MyBuddy;
	void createGui();
	void updateOrder();

private slots:
	void addAccountDataRow(ContactAccountData *data = 0);
	void showOrderDialog();
	void updateOrderAndClose();

public:
	BuddyGeneralConfigurationWidget(Buddy &buddy, QWidget *parent = 0);
	~BuddyGeneralConfigurationWidget();

	void saveConfiguration();

};

#endif // BUDDY_GENERAL_CONFIGURATION_WIDGET_H
