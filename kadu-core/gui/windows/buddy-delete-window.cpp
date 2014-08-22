/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "buddies/buddy-additional-data-delete-handler-manager.h"
#include "buddies/buddy-additional-data-delete-handler.h"
#include "buddies/buddy-manager.h"
#include "icons/kadu-icon.h"
#include "protocols/roster.h"

#include "buddy-delete-window.h"

BuddyDeleteWindow::BuddyDeleteWindow(const BuddySet &buddiesToDelete, QWidget *parent) :
		QDialog(parent), BuddiesToDelete(buddiesToDelete)
{
	setWindowRole("kadu-buddy-delete");

	setAttribute(Qt::WA_DeleteOnClose);
	setModal(false);

	createGui();

	setMaximumHeight(250);
}

BuddyDeleteWindow::~BuddyDeleteWindow()
{

}

void BuddyDeleteWindow::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *topWidget = new QWidget(this);
	mainLayout->addWidget(topWidget);

	QHBoxLayout *topLayout = new QHBoxLayout(topWidget);

	QLabel *iconLabel = new QLabel(topWidget);
	iconLabel->setPixmap(KaduIcon("dialog-warning").icon().pixmap(32, 32));
	topLayout->addWidget(iconLabel, 0, Qt::AlignTop);

	QWidget *contentWidget = new QWidget(topWidget);
	topLayout->addWidget(contentWidget);

	QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);

	QLabel *messageLabel = new QLabel(tr("The following buddies will be deleted:<br/>%1.<br/>Are you sure?").arg(getBuddiesNames()), contentWidget);
	messageLabel->setTextFormat(Qt::RichText);
	messageLabel->setWordWrap(true);
	contentLayout->addWidget(messageLabel);

	QLabel *additionalDataLabel = new QLabel(tr("Please select additional data that will be removed:"), contentWidget);
	contentLayout->addWidget(additionalDataLabel);

	AdditionalDataListView = new QListWidget(contentWidget);
	contentLayout->addWidget(AdditionalDataListView);
	connect(AdditionalDataListView, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(additionalDataListViewItemPressed(QListWidgetItem *)));
	connect(AdditionalDataListView, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(additionalDataListViewItemClicked(QListWidgetItem *)));

	fillAdditionalDataListView();

	QPushButton *deleteButton = new QPushButton(tr("Delete"));
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"));
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
	foreach (BuddyAdditionalDataDeleteHandler *handler, BuddyAdditionalDataDeleteHandlerManager::instance()->items())
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
	foreach (const Buddy &buddy, BuddiesToDelete)
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
			BuddyAdditionalDataDeleteHandler *handler = BuddyAdditionalDataDeleteHandlerManager::instance()->byName(deleteHandlerName);
			if (handler)
				handler->deleteBuddyAdditionalData(buddy);
		}
	}

	QList<Contact> contacts = buddy.contacts();

	// this set owner buddy on all of the contacts
	BuddyManager::instance()->removeItem(buddy);

	foreach (const Contact &contact, contacts)
		Roster::instance()->removeContact(contact);
}

void BuddyDeleteWindow::accept()
{
	QDialog::accept();

	foreach (const Buddy &buddy, BuddiesToDelete)
		deleteBuddy(buddy);
	BuddyManager::instance()->ensureStored();
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

#include "moc_buddy-delete-window.cpp"
