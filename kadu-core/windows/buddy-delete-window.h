/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <QtWidgets/QDialog>
#include <QtWidgets/QListWidgetItem>
#include <injeqt/injeqt.h>

#include "buddies/buddy-set.h"

class QListWidget;

class BuddyAdditionalDataDeleteHandlerManager;
class BuddyManager;
class IconsManager;
class Roster;

class BuddyDeleteWindow : public QDialog
{
    Q_OBJECT

    QPointer<BuddyAdditionalDataDeleteHandlerManager> m_buddyAdditionalDataDeleteHandlerManager;
    QPointer<BuddyManager> m_buddyManager;
    QPointer<IconsManager> m_iconsManager;
    QPointer<Roster> m_roster;
    BuddySet BuddiesToDelete;

    QListWidget *AdditionalDataListView;
    Qt::CheckState ItemState;

    void createGui();
    void fillAdditionalDataListView();

    QString getBuddiesNames();
    void deleteBuddy(Buddy buddy);

private slots:
    INJEQT_SET void setBuddyAdditionalDataDeleteHandlerManager(
        BuddyAdditionalDataDeleteHandlerManager *buddyAdditionalDataDeleteHandlerManager);
    INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_SET void setRoster(Roster *roster);
    INJEQT_INIT void init();

    void additionalDataListViewItemPressed(QListWidgetItem *item);
    void additionalDataListViewItemClicked(QListWidgetItem *item);

public:
    explicit BuddyDeleteWindow(const BuddySet &buddiesToDelete, QWidget *parent = nullptr);
    virtual ~BuddyDeleteWindow();

public slots:
    virtual void accept();
    virtual void reject();
};
