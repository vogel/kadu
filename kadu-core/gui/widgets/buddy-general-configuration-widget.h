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

#include <QtGui/QWidget>

#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "contacts/model/buddy-contact-model.h"

#include "exports.h"

class QDialog;
class QLabel;
class QLineEdit;

class BuddyContactsTable;
class Contact;
class ContactManager;

class KADUAPI BuddyGeneralConfigurationWidget : public QWidget
{
	Q_OBJECT

	QLineEdit *DisplayEdit;
	QLineEdit *PhoneEdit;
	QLineEdit *MobileEdit;
	QLineEdit *EmailEdit;
	QLineEdit *WebsiteEdit;
	QLabel *AvatarLabel;

	BuddyContactsTable *ContactsTable;

	QDialog *OrderDialog;

	Buddy MyBuddy;
	BuddyContactModel *AccountDataModel;

	void createGui();
	void updateOrder();

private slots:
	void changeAvatar();
	void showOrderDialog();
	void updateOrderAndClose();

public:
	explicit BuddyGeneralConfigurationWidget(Buddy &buddy, QWidget *parent = 0);
	virtual ~BuddyGeneralConfigurationWidget();

	bool isValid();
	void save();

signals:
	void validChanged();

};

#endif // BUDDY_GENERAL_CONFIGURATION_WIDGET_H
