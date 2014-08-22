/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>

#include "icons/icons-manager.h"

#include "notify-group-box.h"

NotifyGroupBox::NotifyGroupBox(Notifier *notificator, const QString &caption, QWidget *parent) :
		QWidget(parent), Notificator(notificator)
{
	new QHBoxLayout(this);

	NotifierCheckBox = new QCheckBox(caption);
	NotifierCheckBox->setIcon(notificator->icon().icon());
	layout()->addWidget(NotifierCheckBox);
	connect(NotifierCheckBox, SIGNAL(clicked(bool)), this, SLOT(toggledSlot(bool)));
	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));
}

void NotifyGroupBox::setChecked(bool checked)
{
	NotifierCheckBox->setChecked(checked);
}

void NotifyGroupBox::addWidget(QWidget *widget)
{
	layout()->addWidget(widget);
	widget->setEnabled(NotifierCheckBox->isChecked());
	connect(NotifierCheckBox, SIGNAL(toggled(bool)), widget, SLOT(setEnabled(bool)));
}

void NotifyGroupBox::toggledSlot(bool toggle)
{
	emit toggled(Notificator, toggle);
}

void NotifyGroupBox::iconThemeChanged()
{
	NotifierCheckBox->setIcon(Notificator->icon().icon());
}

#include "moc_notify-group-box.cpp"
