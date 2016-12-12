/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2014 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "buddies/buddy.h"
#include "os/generic/desktop-aware-object.h"
#include "exports.h"

#include <QtWidgets/QDialog>
#include <injeqt/injeqt.h>

class AccountsComboBox;
class BuddyManager;
class BuddyPreferredManager;
class BuddyStorage;
class Configuration;
class ContactManager;
class GroupsComboBox;
class IconsManager;
class InjectedFactory;
class Myself;
class Roster;
class SelectTalkableComboBox;
class TalkableConverter;
class UrlHandlerManager;

class QCheckBox;
class QFormLayout;
class QLabel;
class QLineEdit;
class QRegExpValidator;

class KADUAPI AddBuddyWindow : public QDialog, DesktopAwareObject
{
	Q_OBJECT

public:
	explicit AddBuddyWindow(QWidget *parent = nullptr, const Buddy &buddy = Buddy::null, bool forceBuddyAccount = false);
	virtual ~AddBuddyWindow();

	void setGroup(Group group);

public slots:
	virtual void accept();

private:
	QPointer<BuddyManager> m_buddyManager;
	QPointer<BuddyStorage> m_buddyStorage;
	QPointer<BuddyPreferredManager> m_buddyPreferredManager;
	QPointer<Configuration> m_configuration;
	QPointer<ContactManager> m_contactManager;
	QPointer<IconsManager> m_iconsManager;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<Myself> m_myself;
	QPointer<Roster> m_roster;
	QPointer<TalkableConverter> m_talkableConverter;
	QPointer<UrlHandlerManager> m_urlHandlerManager;

	QFormLayout *Layout;

	QLabel *UserNameLabel;
	QLineEdit *UserNameEdit;
	QAction *MobileAccountAction; // TODO: hack
	QAction *EmailAccountAction; // TODO: hack
	Account LastSelectedAccount;
	AccountsComboBox *AccountCombo;
	GroupsComboBox *GroupCombo;
	QLineEdit *DisplayNameEdit;
	QCheckBox *MergeBuddy;
	SelectTalkableComboBox *SelectBuddy;
	QCheckBox *AskForAuthorization;
	QCheckBox *AllowToSeeMeCheck;
	QLabel *ErrorLabel;
	QPushButton *AddContactButton;

	QList<QWidget *> NonMergeWidgets;
	QList<QWidget *> MergeWidgets;

	Buddy MyBuddy;
	Account MyAccount;
	bool ForceBuddyAccount;

	void createGui();
	void addFakeAccountsToComboBox();
	void displayErrorMessage(const QString &message);

	bool isMobileAccount();
	bool isEmailAccount();

	void updateAccountGui();
	void updateMobileGui();
	void updateEmailGui();

	void validateData();
	void validateMobileData();
	void validateEmailData();

	bool addContact();
	bool addMobile();
	bool addEmail();

	void askForAuthorization(const Contact &contact);
	void sendAuthorization(const Contact &contact);

private slots:
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager);
	INJEQT_SET void setBuddyStorage(BuddyStorage *buddyStorage);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setMyself(Myself *myself);
	INJEQT_SET void setRoster(Roster *roster);
	INJEQT_SET void setTalkableConverter(TalkableConverter *talkableConverter);
	INJEQT_SET void setUrlHandlerManager(UrlHandlerManager *urlHandlerManager);
	INJEQT_INIT void init();

	void accountChanged();
	void updateGui();
	void setAddContactEnabled();
	void mergeToggled(bool toggled);

};
