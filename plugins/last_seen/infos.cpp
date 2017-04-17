/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>

#include "accounts/account-manager.h"
#include "contacts/contact-manager.h"
#include "exports.h"
#include "menu/menu-inventory.h"
#include "misc/paths-provider.h"
#include "plugin/plugin-injected-factory.h"

#include "infos_dialog.h"
#include "show-infos-window-action.h"

#include "infos.h"
#include "infos.moc"

Infos::Infos(QObject *parent) : QObject{parent}, menuID{}
{
}

Infos::~Infos()
{
}

void Infos::setAccountManager(AccountManager *accountManager)
{
    m_accountManager = accountManager;
}

void Infos::setContactManager(ContactManager *contactManager)
{
    m_contactManager = contactManager;
}

void Infos::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
    m_pluginInjectedFactory = pluginInjectedFactory;
}

void Infos::setMenuInventory(MenuInventory *menuInventory)
{
    m_menuInventory = menuInventory;
}

void Infos::setPathsProvider(PathsProvider *pathsProvider)
{
    m_pathsProvider = pathsProvider;
}

void Infos::setShowInfosWindowAction(ShowInfosWindowAction *showInfosWindowAction)
{
    m_showInfosWindowAction = showInfosWindowAction;
}

void Infos::init()
{
    triggerAllAccountsAdded(m_accountManager);

    fileName = m_pathsProvider->profilePath() + QStringLiteral("last_seen.data");

    if (QFile::exists(fileName))
    {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&file);
            while (!stream.atEnd())
            {
                QStringList fullId = stream.readLine().split(':', QString::SkipEmptyParts);
                if (fullId.count() != 2)
                    continue;
                QString protocol = fullId[0];
                QString uin = fullId[1];
                QString dateTime = stream.readLine();

                Contact contact;
                // wstawiamy tylko konta, które są na liście kontaktów
                for (auto account : m_accountManager->byProtocolName(protocol))
                {
                    contact = m_contactManager->byId(account, uin, ActionReturnNull);
                    if (contact.isNull())
                        continue;
                    if (!contact.isAnonymous())
                    {
                        lastSeen[qMakePair(protocol, uin)] = dateTime;
                        // wystarczy, że kontakt jest na jednym koncie, omijamy resztę
                        continue;
                    }
                }
                QString tmp = stream.readLine();   // skip empty line
            }
            file.close();
        }
        else
        {
            fprintf(stderr, "cannot open '%s': %s\n", qPrintable(file.fileName()), qPrintable(file.errorString()));
            fflush(stderr);
        }
    }

    // Main menu entry
    m_menuInventory->menu("tools")->addAction(m_showInfosWindowAction, KaduMenu::SectionTools, 3)->update();
}

void Infos::done()
{
    updateTimes();
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream stream(&file);
        for (LastSeen::Iterator it = lastSeen.begin(); it != lastSeen.end(); ++it)
        {
            QPair<QString, QString> lastSeenKey = it.key();
            stream << lastSeenKey.first << ":" << lastSeenKey.second << "\n" << it.value() << "\n\n";
        }
        file.close();
    }
    else
    {
        fprintf(stderr, "cannot open '%s': %s\n", qPrintable(file.fileName()), qPrintable(file.errorString()));
        fflush(stderr);
    }

    m_menuInventory->menu("tools")->removeAction(m_showInfosWindowAction)->update();
}

void Infos::onShowInfos()
{
    updateTimes();
    InfosDialog *infosDialog = m_pluginInjectedFactory->makeInjected<InfosDialog>(lastSeen);
    infosDialog->show();
}

void Infos::accountAdded(Account account)
{
    connect(account, SIGNAL(buddyStatusChanged(Contact, Status)), this, SLOT(contactStatusChanged(Contact, Status)));
}

void Infos::accountRemoved(Account account)
{
    disconnect(account, 0, this, 0);
}

void Infos::contactStatusChanged(Contact contact, Status status)
{
    Q_UNUSED(status)
    // interesuje nas tylko zmiana na offline, lastSeen dla ludzi online
    // zostanie zapisany przy wyjściu z programu
    if (contact.currentStatus().isDisconnected())
        lastSeen[qMakePair(contact.contactAccount().protocolName(), contact.id())] =
            QDateTime::currentDateTime().toString(QStringLiteral("dd-MM-yyyy hh:mm"));
}

void Infos::updateTimes()
{
    for (auto const &contact : m_contactManager->items())
        if (!contact.currentStatus().isDisconnected())
            lastSeen[qMakePair(contact.contactAccount().protocolName(), contact.id())] =
                QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm");
}
