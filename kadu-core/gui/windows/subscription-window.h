/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SUBSCRIPTION_WINDOW_H
#define SUBSCRIPTION_WINDOW_H

#include <QtGui/QComboBox>
#include <QtGui/QDialog>

#include "contacts/contact.h"

class QLineEdit;

class SubscriptionWindow : public QDialog
{
	Q_OBJECT

	Contact CurrentContact;
	QLineEdit *VisibleName;
	QComboBox *GroupCombo;

private slots:
	void accepted();
	void groupChanged(int index);

public:
	static void getSubscription(Contact contact, QObject* receiver, const char* slot);
	
	explicit SubscriptionWindow(Contact contact, QWidget* parent = 0);
	virtual ~SubscriptionWindow();

signals:
	void requestAccepted(Contact contact);

};

#endif // SUBSCRIPTION_WINDOW_H
