/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QFileDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "buddies/filter/account-buddy-filter.h"
#include "buddies/filter/anonymous-buddy-filter.h"
#include "buddies/model/buddies-model.h"
#include "buddies/model/buddies-model-proxy.h"
#include "contacts/contact.h"
#include "contacts/contact-shared.h"
#include "contacts/contact-details.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/buddies-list-widget.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/windows/message-dialog.h"

#include "debug.h"
#include "protocols/protocol.h"
#include "protocols/services/contact-list-service.h"

#include "account-buddy-list-widget.h"

AccountBuddyListWidget::AccountBuddyListWidget(Account account, QWidget *parent) :
		QWidget(parent), CurrentAccount(account)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(5);

	BuddiesWidget = new BuddiesListWidget(BuddiesListWidget::FilterAtTop, 0, this);
	BuddiesModelProxy *model = new BuddiesModelProxy(this);
	model->setSourceModel(new BuddiesModel(this));
	BuddiesWidget->view()->setModel(model);
	BuddiesWidget->setMinimumSize(QSize(30, 30));

	QWidget *buttons = new QWidget(this);
	QHBoxLayout *buttonsLayout = new QHBoxLayout(buttons);
	buttonsLayout->setContentsMargins(0, 0, 0, 0);
	buttonsLayout->setSpacing(5);

	ImportButton = new QPushButton(tr("Import contacts"), buttons);
	connect(ImportButton, SIGNAL(clicked()), this, SLOT(startImportTransfer()));
	buttonsLayout->addWidget(ImportButton);

	ExportButton = new QPushButton(tr("Export contacts"), buttons);
	connect(ExportButton, SIGNAL(clicked()), this, SLOT(startExportTransfer()));
	buttonsLayout->addWidget(ExportButton);

	QPushButton *restoreFromFile = new QPushButton(tr("Restore from file"), buttons);
	connect(restoreFromFile, SIGNAL(clicked()), this, SLOT(restoreFromFile()));
	buttonsLayout->addWidget(restoreFromFile);

	QPushButton *storeToFile = new QPushButton(tr("Store to file"), buttons);
	connect(storeToFile, SIGNAL(clicked()), this, SLOT(storeToFile()));
	buttonsLayout->addWidget(storeToFile);

	layout->addWidget(BuddiesWidget);
	layout->addWidget(buttons);

	AccountBuddyFilter *accountFilter = new AccountBuddyFilter(CurrentAccount, this);
	accountFilter->setEnabled(true);
	AnonymousBuddyFilter *anonymousFilter = new AnonymousBuddyFilter(this);
	anonymousFilter->setEnabled(true);

	BuddiesWidget->view()->addFilter(accountFilter);
	BuddiesWidget->view()->addFilter(anonymousFilter);

	ContactListService *manager = CurrentAccount.protocolHandler()->contactListService();
	if (!manager)
	{
		ImportButton->setEnabled(false);
		ExportButton->setEnabled(false);
		return;
	}

	connect(manager, SIGNAL(contactListExported(bool)), this, SLOT(buddiesListExported(bool)));
	connect(manager, SIGNAL(contactListImported(bool, BuddyList)),
		this, SLOT(buddiesListImported(bool, BuddyList)));
}

void AccountBuddyListWidget::startImportTransfer()
{
	kdebugf();

	if (!CurrentAccount.protocolHandler()->isConnected())
	{
		MessageDialog::show("dialog-error", tr("Kadu"), tr("Cannot import user list from server in offline mode"));
		return;
	}

	ContactListService *service = CurrentAccount.protocolHandler()->contactListService();
	if (!service)
		return;
	ImportButton->setEnabled(false);
	service->importContactList();

	kdebugf2();
}

void AccountBuddyListWidget::startExportTransfer()
{
	kdebugf();

	if (!CurrentAccount.protocolHandler()->isConnected())
	{
		MessageDialog::show("dialog-error", tr("Kadu"), tr("Cannot export user list to server in offline mode"));
		kdebugf2();
		return;
	}

	ContactListService *service = CurrentAccount.protocolHandler()->contactListService();
	if (!service)
		return;

	ExportButton->setEnabled(false);
	service->exportContactList();

	kdebugf2();
}

void AccountBuddyListWidget::buddiesListImported(bool ok, BuddyList buddies)
{
	kdebugf();

	ImportButton->setEnabled(true);
	if (!ok)
	{
		int result = MessageDialog::exec("dialog-error", tr("Kadu"),
				 tr("Contacts list couldn't be downloaded. Please check that account %0 is connected.").arg(CurrentAccount.id()),
				QMessageBox::Cancel | QMessageBox::Retry);

		if (result == QMessageBox::Retry)
			startImportTransfer();
		return;
	}

	ContactListService *service = CurrentAccount.protocolHandler()->contactListService();
	if (!service)
		return;

	service->setBuddiesList(buddies);
}

void AccountBuddyListWidget::buddiesListExported(bool ok)
{
	kdebugf();

	if (ok)
		MessageDialog::show("dialog-information", tr("Kadu"), tr("Your userlist has been successfully exported to server"));
	else
		MessageDialog::show("dialog-error", tr("Kadu"), tr("The application encountered an internal error\nThe export was unsuccessful"));

	ExportButton->setEnabled(true);

	kdebugf2();
}

void AccountBuddyListWidget::restoreFromFile()
{
	ContactListService *service = CurrentAccount.protocolHandler()->contactListService();
	if (!service)
		return;

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"), "", "Contact list files (*.txt *.xml)", 0);
	if (fileName.isEmpty())
		return;

	QFile file(fileName);

	if (file.exists() && file.open(QFile::ReadOnly))
	{
		QTextStream stream(file.readAll());
		file.close();

		QList<Buddy> list = service->loadBuddyList(stream);

		if (list.isEmpty())
		{
			MessageDialog::exec("dialog-error", tr("Kadu"),
							tr("Contacts list couldn't be imported. File %0 doesn't contain correct contacts list.").arg(fileName),
							QMessageBox::Cancel | QMessageBox::Retry);
			return;
		}

		service->setBuddiesList(list, false);
	}
}

void AccountBuddyListWidget::storeToFile()
{
	ContactListService *service = CurrentAccount.protocolHandler()->contactListService();
	if (!service)
		return;

	QString fileName = QFileDialog::getSaveFileName(this, tr("Select file"), "", "Contact list files (*.txt)", 0);
	if (fileName.isEmpty())
		return;

	QFile file(fileName);

	if (file.open(QFile::WriteOnly))
	{
		file.write(service->storeBuddyList(BuddyManager::instance()->buddies(CurrentAccount)));
		file.close();
	}
}
