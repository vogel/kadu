/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "contacts/contact.h"
#include "os/generic/desktop-aware-object.h"
#include "exports.h"

#include <QtCore/QObject>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <injeqt/injeqt.h>

class BuddyManager;
class ContactManager;
class InjectedFactory;
class PluginInjectedFactory;

class KADUAPI SubscriptionWindow : public QDialog, DesktopAwareObject
{
	Q_OBJECT

public:
	static void getSubscription(PluginInjectedFactory *pluginInjectedFactory, Contact contact, QObject* receiver, const char* slot);

	explicit SubscriptionWindow(Contact contact, QWidget* parent = nullptr);
	virtual ~SubscriptionWindow();

signals:
	void requestConsidered(Contact contact, bool accepted);

private:
	QPointer<BuddyManager> m_buddyManager;
	QPointer<ContactManager> m_contactManager;
	QPointer<InjectedFactory> m_injectedFactory;

	Contact m_contact;

private slots:
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

	void accepted();
	void allowed();
	void rejected();

};
