/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "show-xml-console-action.h"

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "actions/action.h"
#include "actions/actions.h"
#include "gui/windows/xml-console.h"
#include "identities/identity.h"
#include "menu/menu-inventory.h"

#include <QtCore/QTimer>
#include <QtWidgets/QMenu>

ShowXmlConsoleAction::ShowXmlConsoleAction(QObject *parent) : ActionDescription(parent)
{
    setType(ActionDescription::TypeMainMenu);
    setName("showXmlConsole");
    setText(tr("Show XML Console"));
}

ShowXmlConsoleAction::~ShowXmlConsoleAction()
{
    // actions will delete their menus
    m_menuInventory->menu("tools")->removeAction(this)->update();
}

void ShowXmlConsoleAction::setAccountManager(AccountManager *accountManager)
{
    m_accountManager = accountManager;
}

void ShowXmlConsoleAction::setMenuInventory(MenuInventory *menuInventory)
{
    m_menuInventory = menuInventory;
}

void ShowXmlConsoleAction::init()
{
    connect(m_accountManager, SIGNAL(accountAdded(Account)), this, SLOT(updateShowXmlConsoleMenu()));
    connect(m_accountManager, SIGNAL(accountRemoved(Account)), this, SLOT(updateShowXmlConsoleMenu()));

    // It is needed bacause of loading protocol plugins before creating GUI.
    // TODO: Fix somehow. Maybe creating all action descriptions could be delayed.
    QTimer::singleShot(0, this, SLOT(insertMenuActionDescription()));
}

void ShowXmlConsoleAction::insertMenuActionDescription()
{
    m_menuInventory->menu("tools")->addAction(this, KaduMenu::SectionTools)->update();
}

void ShowXmlConsoleAction::actionInstanceCreated(Action *action)
{
    Q_UNUSED(action)

    // It may look like it was suboptimal but in reality there will be
    // only one action instance.
    updateShowXmlConsoleMenu();
}

void ShowXmlConsoleAction::actionTriggered(QAction *sender, bool toggled)
{
    Q_UNUSED(toggled)

    menuActionTriggered(sender);
}

void ShowXmlConsoleAction::updateShowXmlConsoleMenu()
{
    QVector<Account> jabberAccounts = m_accountManager->byProtocolName("jabber");

    foreach (Action *action, actions())
    {
        QMenu *menu = action->menu();
        if (jabberAccounts.isEmpty() || 1 == m_accountManager->items().count())
        {
            delete menu;
            action->setMenu(0);

            if (jabberAccounts.isEmpty())
            {
                action->setData(QVariant());
                action->setVisible(false);
            }
            else
            {
                action->setData(QVariant::fromValue(jabberAccounts.at(0)));
                action->setVisible(true);
            }
        }
        else
        {
            if (menu)
                menu->clear();
            else
            {
                menu = new QMenu();
                action->setMenu(menu);
                connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(menuActionTriggered(QAction *)));
            }

            foreach (const Account &account, jabberAccounts)
            {
                QAction *menuAction =
                    menu->addAction(QString("%1 (%2)").arg(account.accountIdentity().name(), account.id()));
                menuAction->setData(QVariant::fromValue(account));
            }

            action->setData(QVariant());
            action->setVisible(true);
        }
    }
}

void ShowXmlConsoleAction::menuActionTriggered(QAction *action)
{
    Account account = action->data().value<Account>();
    if (!account)
        return;

    (new XmlConsole(account))->show();
}

#include "moc_show-xml-console-action.cpp"
