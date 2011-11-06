/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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
#include "buddies/filter/account-buddy-filter.h"
#include "buddies/filter/anonymous-buddy-filter.h"
#include "buddies/filter/buddy-name-filter.h"
#include "buddies/model/buddies-model.h"
#include "buddies/model/buddies-model-proxy.h"
#include "contacts/contact.h"
#include "contacts/contact-details.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/windows/message-dialog.h"
#include "model/model-chain.h"

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

	ModelChain *chain = new ModelChain(new BuddiesModel(this), this);
	BuddiesModelProxy *proxyModel = new BuddiesModelProxy(chain);

	AccountBuddyFilter *accountFilter = new AccountBuddyFilter(CurrentAccount, proxyModel);
	accountFilter->setEnabled(true);
	AnonymousBuddyFilter *anonymousFilter = new AnonymousBuddyFilter(proxyModel);
	anonymousFilter->setEnabled(true);

	proxyModel->addFilter(accountFilter);
	proxyModel->addFilter(anonymousFilter);

	chain->addProxyModel(proxyModel);

	BuddiesWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, this);

	BuddyNameFilter *nameFilter = new BuddyNameFilter(proxyModel);
	connect(BuddiesWidget, SIGNAL(filterChanged(QString)), nameFilter, SLOT(setName(QString)));
	proxyModel->addFilter(nameFilter);

	BuddiesListView *view = new BuddiesListView(BuddiesWidget);
	view->setChain(chain);

	BuddiesWidget->setTreeView(view);
	BuddiesWidget->setMinimumSize(QSize(30, 30));

	QWidget *buttons = new QWidget(this);
	QHBoxLayout *buttonsLayout = new QHBoxLayout(buttons);
	buttonsLayout->setContentsMargins(0, 0, 0, 0);
	buttonsLayout->setSpacing(5);

	QPushButton *restoreFromFile = new QPushButton(tr("Restore from file"), buttons);
	connect(restoreFromFile, SIGNAL(clicked()), this, SLOT(restoreFromFile()));
	buttonsLayout->addWidget(restoreFromFile);

	QPushButton *storeToFile = new QPushButton(tr("Store to file"), buttons);
	connect(storeToFile, SIGNAL(clicked()), this, SLOT(storeToFile()));
	buttonsLayout->addWidget(storeToFile);

	layout->addWidget(BuddiesWidget);
	layout->addWidget(buttons);
}

void AccountBuddyListWidget::restoreFromFile()
{
	ContactListService *service = CurrentAccount.protocolHandler()->contactListService();
	if (!service)
		return;

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"), QString(), tr("Contact List Files (*.txt *.xml);;All Files (*)"), 0);
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
			MessageDialog::exec(KaduIcon("dialog-error"), tr("Kadu"),
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

	QString fileName = QFileDialog::getSaveFileName(this, tr("Select file"), QString(), tr("Contact List Files (*.txt)"), 0);
	if (fileName.isEmpty())
		return;

	QFile file(fileName);

	if (file.open(QFile::WriteOnly))
	{
		file.write(service->storeBuddyList(BuddyManager::instance()->buddies(CurrentAccount)));
		file.close();
	}
}
