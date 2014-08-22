/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDY_DELETE_WINDOW_H
#define BUDDY_DELETE_WINDOW_H

#include <QtGui/QDialog>
#include <QtGui/QListWidgetItem>

#include "buddies/buddy-set.h"

class QListWidget;

class BuddyDeleteWindow : public QDialog
{
	Q_OBJECT

	BuddySet BuddiesToDelete;

	QListWidget *AdditionalDataListView;
	Qt::CheckState ItemState;

	void createGui();
	void fillAdditionalDataListView();

	QString getBuddiesNames();
	void deleteBuddy(Buddy buddy);

private slots:
	void additionalDataListViewItemPressed(QListWidgetItem *item);
	void additionalDataListViewItemClicked(QListWidgetItem *item);

public:
	explicit BuddyDeleteWindow(const BuddySet &buddiesToDelete, QWidget *parent = 0);
	virtual ~BuddyDeleteWindow();

public slots:
	virtual void accept();
	virtual void reject();

};

#endif // BUDDY_DELETE_WINDOW_H
