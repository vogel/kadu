/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "subscription-window.h"

#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "core/injected-factory.h"
#include "icons/icons-manager.h"
#include "model/roles.h"
#include "plugin/plugin-injected-factory.h"
#include "roster/roster-entry-state.h"
#include "roster/roster-entry.h"
#include "windows/add-buddy-window.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

void SubscriptionWindow::getSubscription(
    PluginInjectedFactory *pluginInjectedFactory, Contact contact, QObject *receiver, const char *slot)
{
    SubscriptionWindow *window = pluginInjectedFactory->makeInjected<SubscriptionWindow>(contact);
    connect(window, SIGNAL(requestConsidered(Contact, bool)), receiver, slot);

    window->exec();
}

SubscriptionWindow::SubscriptionWindow(Contact contact, QWidget *parent)
        : QDialog{parent}, DesktopAwareObject{this}, m_contact{contact}
{
    setWindowRole("kadu-subscription");

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Ask For Sharing Status"));
    resize(500, 120);
}

SubscriptionWindow::~SubscriptionWindow()
{
}

void SubscriptionWindow::setBuddyManager(BuddyManager *buddyManager)
{
    m_buddyManager = buddyManager;
}

void SubscriptionWindow::setContactManager(ContactManager *contactManager)
{
    m_contactManager = contactManager;
}

void SubscriptionWindow::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void SubscriptionWindow::init()
{
    // It'd be too unsafe to not add this contact to the manager now and rely later on addItem()
    // as the contact might be added in the meantime. See bug #2222.
    auto knownContact = m_contactManager->byId(m_contact.contactAccount(), m_contact.id(), ActionReturnNull);
    if (knownContact)
        m_contact = knownContact;

    auto layout = new QGridLayout(this);
    layout->setColumnStretch(2, 4);

    auto messageLabel =
        new QLabel(tr("User <b>%1</b> wants to add you to his contact list.").arg(m_contact.id()), this);
    auto finalQuestionLabel = new QLabel(tr("Do you want this person to see your status?"), this);
    auto buttons = new QDialogButtonBox(Qt::Horizontal, this);
    auto shareAndAdd =
        new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Allow and add buddy..."), this);

    if (knownContact && !knownContact.isAnonymous())
    {
        shareAndAdd->setVisible(false);
    }
    else
    {
        shareAndAdd->setDefault(true);
        buttons->addButton(shareAndAdd, QDialogButtonBox::AcceptRole);
    }

    auto share = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Allow"), this);
    share->setDefault(true);
    buttons->addButton(share, QDialogButtonBox::AcceptRole);

    auto cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Ignore"), this);
    buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

    connect(shareAndAdd, SIGNAL(clicked(bool)), this, SLOT(accepted()));
    connect(share, SIGNAL(clicked(bool)), this, SLOT(allowed()));
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(rejected()));

    layout->addWidget(messageLabel, 0, 0, 1, 3);
    layout->addWidget(finalQuestionLabel, 1, 0, 1, 1);
    layout->addWidget(buttons, 2, 0, 1, 3);

    shareAndAdd->setFocus();
}

void SubscriptionWindow::accepted()
{
    auto buddy = m_buddyManager->byContact(m_contact, ActionCreate);
    buddy.setAnonymous(true);
    (m_injectedFactory->makeInjected<AddBuddyWindow>(nullptr, buddy))->show();
    allowed();
}

void SubscriptionWindow::allowed()
{
    emit requestConsidered(m_contact, true);
    close();
}

void SubscriptionWindow::rejected()
{
    emit requestConsidered(m_contact, false);
    close();
}

#include "moc_subscription-window.cpp"
