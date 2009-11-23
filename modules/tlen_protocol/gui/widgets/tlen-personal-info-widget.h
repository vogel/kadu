 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_PERSONAL_INFO_WIDGET_H
#define TLEN_PERSONAL_INFO_WIDGET_H

#include <QtGui/QWidget>

#include "buddies/buddy.h"

class QComboBox;
class QCheckBox;
class QLineEdit;

class Account;
class PersonalInfoService;

class TlenPersonalInfoWidget : public QWidget
{
	Q_OBJECT

	PersonalInfoService *Service;
	Account TAccount;

	QLineEdit *NickName;
	QLineEdit *FirstName;
	QLineEdit *LastName;
	QComboBox *Sex;
	QLineEdit *FamilyName;
	QLineEdit *BirthYear;
	QLineEdit *City;

	QComboBox *LookingFor;
	QComboBox *Job;
	QComboBox *TodayPlans;

	QCheckBox *ShowStatus;
	QCheckBox *HaveMic;
	QCheckBox *HaveCam;

	void createGui();

private slots:
	void personalInfoAvailable(Buddy buddy);

public:
	explicit TlenPersonalInfoWidget(Account account, QWidget *parent = 0);
	virtual ~TlenPersonalInfoWidget();

	void applyData();

};

#endif // TLEN_PERSONAL_INFO_WIDGET_H
