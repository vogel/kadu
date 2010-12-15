/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-additional-data-delete-handler.h"
#include "buddies/buddy-additional-data-delete-handler-manager.h"
#include "buddies/buddy-manager.h"
#include "icons-manager.h"

#include "buddy-delete-window.h"

BuddyDeleteWindow::BuddyDeleteWindow(const BuddySet &buddiesToDelete, QWidget *parent) :
		QDialog(parent), BuddiesToDelete(buddiesToDelete)
{
	setWindowRole("kadu-buddy-delete");

	setAttribute(Qt::WA_DeleteOnClose);
	setModal(false);

	createGui();
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
	iconLabel->setPixmap(IconsManager::instance()->iconByPath("dialog-warning").pixmap(32, 32));
	topLayout->addWidget(iconLabel, 0, Qt::AlignTop);

	QWidget *contentWidget = new QWidget(topWidget);
	topLayout->addWidget(contentWidget);

	QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);

	QLabel *messageLabel = new QLabel(tr("Selected users:\n%0 will be deleted. Are you sure?").arg(getBuddiesNames()), contentWidget);
	messageLabel->setTextFormat(Qt::RichText);
	messageLabel->setWordWrap(true);
	contentLayout->addWidget(messageLabel);

	QLabel *additionalDataLabel = new QLabel(tr("Select additional data that should be removed:"), contentWidget);
	contentLayout->addWidget(additionalDataLabel);

	AdditionalDataListView = new QListWidget(contentWidget);
	contentLayout->addWidget(AdditionalDataListView);

	fillAdditionalDataListView();

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	mainLayout->addWidget(buttons);

	QPushButton *deleteButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogDiscardButton), tr("Delete"));
	deleteButton->setAutoDefault(true);
	deleteButton->setDefault(true);

	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"));

	connect(deleteButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(deleteButton, QDialogButtonBox::DestructiveRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
}

void BuddyDeleteWindow::fillAdditionalDataListView()
{
	foreach (BuddyAdditionalDataDeleteHandler *handler, BuddyAdditionalDataDeleteHandlerManager::instance()->items())
	{
		QListWidgetItem *item = new QListWidgetItem(AdditionalDataListView);
		item->setText(handler->displayName());
		item->setCheckState(Qt::Checked);
		item->setData(Qt::UserRole, handler->name());

		AdditionalDataListView->addItem(item);
	}
}

QString BuddyDeleteWindow::getBuddiesNames()
{
	QStringList displays;
	foreach (const Buddy &buddy, BuddiesToDelete)
		displays.append(QString("<b>%0</b>").arg(buddy.display()));

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

	foreach (Contact contact, buddy.contacts())
		contact.setOwnerBuddy(Buddy::null);

	BuddyManager::instance()->removeItem(buddy);
}

void BuddyDeleteWindow::accept()
{
    QDialog::accept();

	foreach (const Buddy &buddy, BuddiesToDelete)
		deleteBuddy(buddy);
	BuddyManager::instance()->store();
}

void BuddyDeleteWindow::reject()
{
    QDialog::reject();
}
