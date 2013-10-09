/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

class QCheckBox;
class QLineEdit;

class BuddyAvatarWidget;
class BuddyContactsTable;
class CompositeConfigurationValueStateNotifier;
class ConfigurationValueStateNotifier;
class Contact;
class ContactManager;
class SimpleConfigurationValueStateNotifier;

class KADUAPI BuddyGeneralConfigurationWidget : public QWidget
{
	Q_OBJECT

	QLineEdit *DisplayEdit;
	QLineEdit *PhoneEdit;
	QLineEdit *MobileEdit;
	QLineEdit *EmailEdit;
	QLineEdit *WebsiteEdit;

	CompositeConfigurationValueStateNotifier *ValueStateNotifier;
	SimpleConfigurationValueStateNotifier *SimpleValueStateNotifier;

	BuddyAvatarWidget *AvatarWidget;
	BuddyContactsTable *ContactsTable;
	QCheckBox *PreferHigherStatusCheck;

	Buddy MyBuddy;

	void createGui();

	void removeBuddyAvatar();
	void setBuddyAvatar(const QPixmap &avatar);

	bool isValid() const;

private slots:
	void updateStateNotifier();

public:
	explicit BuddyGeneralConfigurationWidget(const Buddy &buddy, QWidget *parent = 0);
	virtual ~BuddyGeneralConfigurationWidget();

	const ConfigurationValueStateNotifier * valueStateNotifier() const;

	void save();

};

#endif // BUDDY_GENERAL_CONFIGURATION_WIDGET_H
