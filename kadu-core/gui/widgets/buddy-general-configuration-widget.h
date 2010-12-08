/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef BUDDY_GENERAL_CONFIGURATION_WIDGET_H
#define BUDDY_GENERAL_CONFIGURATION_WIDGET_H

#include <QtGui/QWidget>

#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "contacts/model/buddy-contact-model.h"

#include "exports.h"

class QLineEdit;

class BuddyAvatarWidget;
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

	BuddyAvatarWidget *AvatarWidget;
	BuddyContactsTable *ContactsTable;

	Buddy MyBuddy;

	void createGui();

	void removeBuddyAvatar();
	void setBuddyAvatar(const QPixmap &avatar);

public:
	explicit BuddyGeneralConfigurationWidget(Buddy &buddy, QWidget *parent = 0);
	virtual ~BuddyGeneralConfigurationWidget();

	bool isValid();
	void save();

signals:
	void validChanged();

};

#endif // BUDDY_GENERAL_CONFIGURATION_WIDGET_H
