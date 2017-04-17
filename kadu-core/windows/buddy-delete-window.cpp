/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Piotr Dąbrowski (ultr@ultr.pl)
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

#include "buddy-delete-window.h"
#include "buddy-delete-window.moc"

#include "buddies/buddy-additional-data-delete-handler-manager.h"
#include "buddies/buddy-additional-data-delete-handler.h"
#include "buddies/buddy-manager.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "roster/roster.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QVBoxLayout>

BuddyDeleteWindow::BuddyDeleteWindow(const BuddySet &buddiesToDelete, QWidget *parent)
        : QDialog(parent), BuddiesToDelete(buddiesToDelete)
{
    setWindowRole("kadu-buddy-delete");

    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);
    setMaximumHeight(250);
}

BuddyDeleteWindow::~BuddyDeleteWindow()
{
}

void BuddyDeleteWindow::setBuddyAdditionalDataDeleteHandlerManager(
    BuddyAdditionalDataDeleteHandlerManager *buddyAdditionalDataDeleteHandlerManager)
{
    m_buddyAdditionalDataDeleteHandlerManager = buddyAdditionalDataDeleteHandlerManager;
}

void BuddyDeleteWindow::setBuddyManager(BuddyManager *buddyManager)
{
    m_buddyManager = buddyManager;
}

void BuddyDeleteWindow::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void BuddyDeleteWindow::setRoster(Roster *roster)
{
    m_roster = roster;
}

void BuddyDeleteWindow::init()
{
    createGui();
}

void BuddyDeleteWindow::createGui()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *topWidget = new QWidget(this);
    mainLayout->addWidget(topWidget);

    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);

    QLabel *iconLabel = new QLabel(topWidget);
    iconLabel->setPixmap(m_iconsManager->iconByPath(KaduIcon("dialog-warning")).pixmap(32, 32));
    topLayout->addWidget(iconLabel, 0, Qt::AlignTop);

    QWidget *contentWidget = new QWidget(topWidget);
    topLayout->addWidget(contentWidget);

    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);

    QLabel *messageLabel = new QLabel(
        tr("The following buddies will be deleted:<br/>%1.<br/>Are you sure?").arg(getBuddiesNames()), contentWidget);
    messageLabel->setTextFormat(Qt::RichText);
    messageLabel->setWordWrap(true);
    contentLayout->addWidget(messageLabel);

    QLabel *additionalDataLabel = new QLabel(tr("Please select additional data that will be removed:"), contentWidget);
    contentLayout->addWidget(additionalDataLabel);

    AdditionalDataListView = new QListWidget(contentWidget);
    contentLayout->addWidget(AdditionalDataListView);
    connect(
        AdditionalDataListView, SIGNAL(itemPressed(QListWidgetItem *)), this,
        SLOT(additionalDataListViewItemPressed(QListWidgetItem *)));
    connect(
        AdditionalDataListView, SIGNAL(itemClicked(QListWidgetItem *)), this,
        SLOT(additionalDataListViewItemClicked(QListWidgetItem *)));

    fillAdditionalDataListView();

    QPushButton *deleteButton = new QPushButton(tr("Delete"));
    QPushButton *cancelButton =
        new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"));
    cancelButton->setDefault(true);

    QDialogButtonBox *buttons = new QDialogButtonBox(this);
    buttons->addButton(deleteButton, QDialogButtonBox::DestructiveRole);
    buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
    mainLayout->addWidget(buttons);

    connect(deleteButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));
}

void BuddyDeleteWindow::fillAdditionalDataListView()
{
    for (auto handler : m_buddyAdditionalDataDeleteHandlerManager->items())
    {
        QListWidgetItem *item = new QListWidgetItem(AdditionalDataListView);
        item->setText(handler->displayName());
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole, handler->name());

        AdditionalDataListView->addItem(item);
    }
}

QString BuddyDeleteWindow::getBuddiesNames()
{
    QStringList displays;
    for (auto const &buddy : BuddiesToDelete)
        displays.append(QString("<b>%1</b>").arg(buddy.display()));

    return displays.join(", ");
}

void BuddyDeleteWindow::deleteBuddy(Buddy buddy)
{
    for (int i = 0; i < AdditionalDataListView->count(); i++)
    {
        QListWidgetItem *item = AdditionalDataListView->item(i);
        if (Qt::Checked == item->checkState())
        {
            QString deleteHandlerName = item->data(Qt::UserRole).toString();
            BuddyAdditionalDataDeleteHandler *handler =
                m_buddyAdditionalDataDeleteHandlerManager->byName(deleteHandlerName);
            if (handler)
                handler->deleteBuddyAdditionalData(buddy);
        }
    }

    QList<Contact> contacts = buddy.contacts();

    // this set owner buddy on all of the contacts
    m_buddyManager->removeItem(buddy);

    for (auto const &contact : contacts)
        m_roster->removeContact(contact);
}

void BuddyDeleteWindow::accept()
{
    QDialog::accept();

    for (auto const &buddy : BuddiesToDelete)
        deleteBuddy(buddy);
    m_buddyManager->ensureStored();
}

void BuddyDeleteWindow::reject()
{
    QDialog::reject();
}

void BuddyDeleteWindow::additionalDataListViewItemPressed(QListWidgetItem *item)
{
    if (!item)
        return;

    // required for proper handling of mouse double clicks
    ItemState = item->checkState();
}

void BuddyDeleteWindow::additionalDataListViewItemClicked(QListWidgetItem *item)
{
    if (!item)
        return;

    ItemState = (ItemState == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);

    item->setCheckState(ItemState);
}
