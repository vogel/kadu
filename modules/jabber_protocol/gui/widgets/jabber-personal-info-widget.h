/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_PERSONAL_INFO_WIDGET_H
#define JABBER_PERSONAL_INFO_WIDGET_H

#include <QtGui/QWidget>

#include "buddies/buddy.h"

class QComboBox;
class QLineEdit;

class Account;
class PersonalInfoService;

class JabberPersonalInfoWidget : public QWidget
{
	Q_OBJECT

	PersonalInfoService *Service;
	Buddy MyBuddy;

	QLineEdit *FullName;
	QLineEdit *NickName;
	QLineEdit *FamilyName;
	QLineEdit *BirthYear;
	QLineEdit *City;
	QLineEdit *Email;
	QLineEdit *Website;

	void createGui();
	void fillForm();

private slots:
	void personalInfoAvailable(Buddy buddy);

public:
	explicit JabberPersonalInfoWidget(Account account, QWidget *parent = 0);
	virtual ~JabberPersonalInfoWidget();

	bool isModified();

	void apply();
	void cancel();

signals:
	void dataChanged();

};

#endif // JABBER_PERSONAL_INFO_WIDGET_H
