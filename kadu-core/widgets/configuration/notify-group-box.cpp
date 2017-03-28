/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "notify-group-box.h"

#include "icons/icons-manager.h"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>

NotifyGroupBox::NotifyGroupBox(Notifier *notificator, const QString &caption, QWidget *parent)
        : QWidget{parent}, m_notificator{notificator}
{
    auto layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    m_notifierCheckBox = make_owned<QCheckBox>(caption, this);
    layout->addWidget(m_notifierCheckBox);
    connect(m_notifierCheckBox, SIGNAL(clicked(bool)), this, SLOT(toggledSlot(bool)));
}

NotifyGroupBox::~NotifyGroupBox()
{
}

void NotifyGroupBox::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void NotifyGroupBox::init()
{
    connect(m_iconsManager, SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));
}

void NotifyGroupBox::setChecked(bool checked)
{
    m_notifierCheckBox->setChecked(checked);
}

void NotifyGroupBox::addWidget(QWidget *widget)
{
    layout()->addWidget(widget);
    widget->setEnabled(m_notifierCheckBox->isChecked());
    connect(m_notifierCheckBox, SIGNAL(toggled(bool)), widget, SLOT(setEnabled(bool)));
}

void NotifyGroupBox::toggledSlot(bool toggle)
{
    emit toggled(m_notificator, toggle);
}

void NotifyGroupBox::iconThemeChanged()
{
    m_notifierCheckBox->setIcon(m_iconsManager->iconByPath(m_notificator->icon()));
}

#include "moc_notify-group-box.cpp"
