/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_INFO_PANEL_H
#define BUDDY_INFO_PANEL_H

#include "configuration/configuration-aware-object.h"
#include "buddies/buddy.h"

#include "kadu-text-browser.h"

class BuddyInfoPanel : public KaduTextBrowser, private ConfigurationAwareObject
{
	Q_OBJECT

	Buddy MyBuddy;
	QString Template;
	QString Syntax;

protected:
	virtual void configurationUpdated();

public:
	explicit BuddyInfoPanel(QWidget *parent = 0);
	virtual ~BuddyInfoPanel();

public slots:
	void displayBuddy(Buddy buddy);
	void styleFixup(QString &syntax);

};

#endif // BUDDY_INFO_PANEL_H
