/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class QComboBox;
class QLineEdit;

class Account;
class BuddyStorage;
class PersonalInfoService;

class GaduPersonalInfoWidget : public QWidget
{
	Q_OBJECT

	QPointer<BuddyStorage> m_buddyStorage;

	Account m_account;
	PersonalInfoService *Service;
	QString Id;
	Buddy MyBuddy;

	QLineEdit *NickName;
	QLineEdit *FirstName;
	QLineEdit *LastName;
	QComboBox *Sex;
	QLineEdit *FamilyName;
	QLineEdit *BirthYear;
	QLineEdit *City;
	QLineEdit *FamilyCity;

	void createGui();
	void fillForm();

private slots:
	INJEQT_SET void setBuddyStorage(BuddyStorage *buddyStorage);
	INJEQT_INIT void init();

	void personalInfoAvailable(Buddy buddy);

public:
	explicit GaduPersonalInfoWidget(Account account, QWidget *parent = nullptr);
	virtual ~GaduPersonalInfoWidget();

	bool isModified();

	void apply();
	void cancel();

signals:
	void dataChanged();

};
