/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
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

#include <QtGui/QHBoxLayout>
#include <QtGui/QCheckBox>

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
