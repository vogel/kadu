/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_OPTIONS_CONFIGURATION_WIDGET_H
#define BUDDY_OPTIONS_CONFIGURATION_WIDGET_H

#include <QtGui/QDialog>
#include <QtGui/QCheckBox>

#include "buddies/buddy.h"
#include "gui/widgets/configuration/configuration-widget.h"

#include "exports.h"

class ContactAccountData;
class ContactAccountDataManager;

class KADUAPI BuddyOptionsConfigurationWidget : public QWidget
{
	Q_OBJECT

	QCheckBox *BlockCheckBox;
	QCheckBox *OfflineToCheckBox;
	QCheckBox *NotifyCheckBox;

	Buddy MyBuddy;
	void createGui();

public:
	explicit BuddyOptionsConfigurationWidget(Buddy &buddy, QWidget *parent = 0);
	virtual ~BuddyOptionsConfigurationWidget();
	void saveConfiguration();

};

#endif // BUDDY_OPTIONS_CONFIGURATION_WIDGET_H
