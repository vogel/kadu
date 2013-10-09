/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "buddies/model/buddy-list-model.h"
#include "buddies/model/buddy-manager-adapter.h"
#include "contacts/contact-details.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/windows/message-dialog.h"
#include "model/model-chain.h"
#include "talkable/filter/account-talkable-filter.h"
#include "talkable/filter/hide-anonymous-talkable-filter.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

#include "protocols/protocol.h"
#include "protocols/services/contact-list-service.h"
#include "debug.h"

#include "account-buddy-list-widget.h"

AccountBuddyListWidget::AccountBuddyListWidget(Account account, QWidget *parent) :
		QWidget(parent), CurrentAccount(account)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(5);

	ModelChain *chain = new ModelChain(this)
;
	BuddyListModel *buddyListModel = new BuddyListModel(chain);
	new BuddyManagerAdapter(buddyListModel);
	chain->setBaseModel(buddyListModel);
	TalkableProxyModel *proxyModel = new TalkableProxyModel(chain);

	AccountTalkableFilter *accountTalkableFilter = new AccountTalkableFilter(proxyModel);
	accountTalkableFilter->setAccount(CurrentAccount);
	proxyModel->addFilter(accountTalkableFilter);

	proxyModel->addFilter(new HideAnonymousTalkableFilter(proxyModel));
	chain->addProxyModel(proxyModel);

	BuddiesWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, this);

	NameTalkableFilter *nameFilter = new NameTalkableFilter(NameTalkableFilter::UndecidedMatching, proxyModel);
	connect(BuddiesWidget, SIGNAL(filterChanged(QString)), nameFilter, SLOT(setName(QString)));
	proxyModel->addFilter(nameFilter);

	TalkableTreeView *view = new TalkableTreeView(BuddiesWidget);
	view->setChain(chain);

	BuddiesWidget->setView(view);
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
			MessageDialog::show(KaduIcon("dialog-error"), tr("Kadu"),
							tr("Contacts list couldn't be imported. File %0 doesn't contain correct contacts list.").arg(fileName));
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

#include "moc_account-buddy-list-widget.cpp"
