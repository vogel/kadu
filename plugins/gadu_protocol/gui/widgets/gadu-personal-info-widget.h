/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef GADU_PERSONAL_INFO_WIDGET_H
#define GADU_PERSONAL_INFO_WIDGET_H

#include <QtGui/QWidget>

#include "buddies/buddy.h"

class QComboBox;
class QLineEdit;

class Account;
class PersonalInfoService;

class GaduPersonalInfoWidget : public QWidget
{
	Q_OBJECT

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
	void personalInfoAvailable(Buddy buddy);

public:
	explicit GaduPersonalInfoWidget(Account account, QWidget *parent = 0);
	virtual ~GaduPersonalInfoWidget();

	bool isModified();

	void apply();
	void cancel();

signals:
	void dataChanged();

};

#endif // GADU_PERSONAL_INFO_WIDGET_H
