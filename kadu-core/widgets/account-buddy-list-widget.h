/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "buddies/buddy-list.h"
#include "exports.h"

#include <QtCore/QPointer>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

class BuddyManager;
class FilteredTreeView;
class IconsManager;
class InjectedFactory;
class RosterReplacer;
class Roster;

class KADUAPI AccountBuddyListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountBuddyListWidget(Account account, QWidget *parent = nullptr);
    virtual ~AccountBuddyListWidget()
    {
    }

private:
    QPointer<BuddyManager> m_buddyManager;
    QPointer<IconsManager> m_iconsManager;
    QPointer<InjectedFactory> m_injectedFactory;
    QPointer<RosterReplacer> m_rosterReplacer;
    QPointer<Roster> m_roster;

    Account CurrentAccount;

    FilteredTreeView *BuddiesWidget;

private slots:
    INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
    INJEQT_SET void setRosterReplacer(RosterReplacer *rosterReplacer);
    INJEQT_SET void setRoster(Roster *roster);
    INJEQT_INIT void init();

    void restoreFromFile();
    void storeToFile();
};
