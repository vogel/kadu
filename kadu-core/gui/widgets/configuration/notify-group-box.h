/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef NOTIFY_GROUP_BOX
#define NOTIFY_GROUP_BOX

#include <QtGui/QWidget>

#include "notify/notifier.h"

class QCheckBox;

class KADUAPI NotifyGroupBox : public QWidget
{
	Q_OBJECT

	QCheckBox *NotifierCheckBox;

	Notifier *Notificator;

private slots:
	void toggledSlot(bool toggled);
	void iconThemeChanged();

public:
	NotifyGroupBox(Notifier *Notificator, const QString &caption, QWidget *parent = 0);
	virtual ~NotifyGroupBox() {}

	Notifier * notificator() { return Notificator; }

	void setChecked(bool checked);
	void addWidget(QWidget *widget);

signals:
	void toggled(Notifier *notificator, bool toggled);

};

#endif // NOTIFY_GROUP_BOX
