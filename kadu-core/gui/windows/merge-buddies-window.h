/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef MERGE_BUDDIES_WINDOW_H
#define MERGE_BUDDIES_WINDOW_H

#include <QtGui/QDialog>

#include "buddies/buddy.h"

class SelectBuddyComboBox;

class MergeBuddiesWindow : public QDialog
{
	Q_OBJECT

	Buddy MyBuddy;

	SelectBuddyComboBox *SelectCombo;
	QPushButton *MergeButton;

	void createGui();

private slots:
	void selectedBuddyChanged(Buddy);

protected slots:
	virtual void accept();

public:
	explicit MergeBuddiesWindow(Buddy buddy, QWidget *parent = 0);
	virtual ~MergeBuddiesWindow();

};

#endif // MERGE_BUDDIES_WINDOW_H
