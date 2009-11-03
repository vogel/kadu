/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_PERSONAL_INFO_CONFIGURATION_WIDGET_H
#define BUDDY_PERSONAL_INFO_CONFIGURATION_WIDGET_H

#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>

#include "buddies/buddy.h"

#include "exports.h"

class KADUAPI BuddyPersonalInfoConfigurationWidget : public QWidget
{
	Q_OBJECT

	QComboBox *ContactIdCombo;
	QLabel *FirstNameText, *LastNameText, *NicknameText, *GenderText, 
	*BirthdateText, *CityText, *StateProvinceText, *IpText, *PortText,
	*DnsNameText, *ProtocolVerText;

	Buddy MyBuddy;
	void createGui();

private slots:
	void accountSelectionChanged(int index);

public:
	BuddyPersonalInfoConfigurationWidget(Buddy &buddy, QWidget *parent = 0);
	~BuddyPersonalInfoConfigurationWidget();

};

#endif // BUDDY_PERSONAL_INFO_CONFIGURATION_WIDGET_H
