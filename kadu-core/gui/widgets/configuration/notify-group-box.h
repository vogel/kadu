/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "misc/memory.h"
#include "notification/notifier.h"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class IconsManager;

class QCheckBox;

class KADUAPI NotifyGroupBox : public QWidget
{
	Q_OBJECT

public:
	explicit NotifyGroupBox(Notifier *Notificator, const QString &caption, QWidget *parent = nullptr);
	virtual ~NotifyGroupBox();

	Notifier * notificator() { return m_notificator; }

	void setChecked(bool checked);
	void addWidget(QWidget *widget);

signals:
	void toggled(Notifier *notificator, bool toggled);

private:
	QPointer<IconsManager> m_iconsManager;

	owned_qptr<QCheckBox> m_notifierCheckBox;
	Notifier *m_notificator;

private slots:
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_INIT void init();

	void toggledSlot(bool toggled);
	void iconThemeChanged();

};
