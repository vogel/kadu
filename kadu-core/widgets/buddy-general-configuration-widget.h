/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "contacts/model/buddy-contact-model.h"
#include "exports.h"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class QCheckBox;
class QLineEdit;

class AvatarManager;
class BuddyAvatarWidget;
class BuddyContactsTable;
class BuddyManager;
class CompositeConfigurationValueStateNotifier;
class ConfigurationValueStateNotifier;
class Contact;
class ContactManager;
class InjectedFactory;
class SimpleConfigurationValueStateNotifier;

class KADUAPI BuddyGeneralConfigurationWidget : public QWidget
{
	Q_OBJECT

public:
	explicit BuddyGeneralConfigurationWidget(const Buddy &buddy, QWidget *parent = nullptr);
	virtual ~BuddyGeneralConfigurationWidget();

	const ConfigurationValueStateNotifier * valueStateNotifier() const;

	void save();

private:
	QPointer<AvatarManager> m_avatarManager;
	QPointer<BuddyManager> m_buddyManager;
	QPointer<InjectedFactory> m_injectedFactory;

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
	INJEQT_SET void setAvatarManager(AvatarManager *avatarManager);
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

	void updateStateNotifier();

};
