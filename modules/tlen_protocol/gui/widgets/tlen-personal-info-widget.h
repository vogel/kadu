/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef TLEN_PERSONAL_INFO_WIDGET_H
#define TLEN_PERSONAL_INFO_WIDGET_H

#include <QtGui/QWidget>

#include "accounts/account.h"
#include "buddies/buddy.h"

class QComboBox;
class QCheckBox;
class QLineEdit;

class PersonalInfoService;

class TlenPersonalInfoWidget : public QWidget
{
	Q_OBJECT

	PersonalInfoService *Service;
	Account TAccount;

	QLineEdit *NickName;
	QLineEdit *FirstName;
	QLineEdit *LastName;
	QLineEdit *EMail;
	QComboBox *Sex;
	QLineEdit *BirthYear;
	QLineEdit *City;

	QComboBox *LookingFor;
	QComboBox *Job;
	QComboBox *TodayPlans;

	QCheckBox *ShowStatus;
	QCheckBox *HaveMic;
	QCheckBox *HaveCam;

	bool FetchOk;

	void createGui();

private slots:
	void personalInfoAvailable(Buddy buddy);

public:
	explicit TlenPersonalInfoWidget(Account account, QWidget *parent = 0);
	virtual ~TlenPersonalInfoWidget();

	void applyData();

};

#endif // TLEN_PERSONAL_INFO_WIDGET_H
