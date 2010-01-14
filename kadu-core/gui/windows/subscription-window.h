/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
