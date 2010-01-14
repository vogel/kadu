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

#ifndef BUDDY_PERSONAL_INFO_CONFIGURATION_WIDGET_H
#define BUDDY_PERSONAL_INFO_CONFIGURATION_WIDGET_H

#include <QtGui/QWidget>

#include "buddies/buddy.h"

#include "exports.h"

class QComboBox;
class QLabel;

class KADUAPI BuddyPersonalInfoConfigurationWidget : public QWidget
{
	Q_OBJECT

	QComboBox *ContactIdCombo;
	QLabel *FirstNameText;
	QLabel *LastNameText;
	QLabel *NicknameText;
	QLabel *GenderText;
	QLabel *BirthdateText;
	QLabel *CityText;
	QLabel *StateProvinceText;
	QLabel *IpText;
	QLabel *PortText;
	QLabel *DnsNameText;
	QLabel *ProtocolVerText;

	Buddy MyBuddy;
	void createGui();

private slots:
	void accountSelectionChanged(int index);

public:
	explicit BuddyPersonalInfoConfigurationWidget(Buddy &buddy, QWidget *parent = 0);
	virtual ~BuddyPersonalInfoConfigurationWidget();

};

#endif // BUDDY_PERSONAL_INFO_CONFIGURATION_WIDGET_H
