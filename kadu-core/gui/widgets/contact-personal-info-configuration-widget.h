/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_PERSONAL_INFO_CONFIGURATION_WIDGET
#define CONTACT_PERSONAL_INFO_CONFIGURATION_WIDGET

#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>

#include "buddies/buddy.h"

#include "exports.h"

class KADUAPI ContactPersonalInfoConfigurationWidget : public QWidget
{
	Q_OBJECT

	QComboBox *ContactIdCombo;
	QLabel *FirstNameText, *LastNameText, *NicknameText, *GenderText, 
	*BirthdateText, *CityText, *StateProvinceText, *IpText, *PortText,
	*DnsNameText, *ProtocolVerText;

	Buddy CurrentContact;
	void createGui();

public:
	ContactPersonalInfoConfigurationWidget(Buddy &contact, QWidget *parent = 0);
	~ContactPersonalInfoConfigurationWidget();

private slots:
	void accountSelectionChanged(int index);

};

#endif // CONTACT_PERSONAL_INFO_CONFIGURATION_WIDGET
