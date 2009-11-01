/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MERGE_BUDDIES_WINDOW_H
#define MERGE_BUDDIES_WINDOW_H

#include <QtGui/QDialog>

#include "buddies/buddy.h"

class SelectBuddyCombobox;

class MergeBuddiesWindow : public QDialog
{
	Q_OBJECT

	Buddy MyBuddy;

	SelectBuddyCombobox *SelectCombo;
	QPushButton *MergeButton;

	void createGui();

private slots:
	void selectedBuddyChanged(Buddy);

protected slots:
	virtual void accept();

public:
	explicit MergeBuddiesWindow(Buddy contact, QWidget *parent = 0);
	virtual ~MergeBuddiesWindow();

};

#endif // MERGE_BUDDIES_WINDOW_H
