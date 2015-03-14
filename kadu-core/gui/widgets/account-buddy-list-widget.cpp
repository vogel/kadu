/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

#include "buddies/buddy-manager.h"
#include "buddies/model/buddy-list-model.h"
#include "buddies/model/buddy-manager-adapter.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/windows/message-dialog.h"
#include "model/model-chain.h"
#include "talkable/filter/account-talkable-filter.h"
#include "talkable/filter/hide-anonymous-talkable-filter.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

#include "protocols/protocol.h"
#include "protocols/services/buddy-list-serialization-service.h"
#include "roster/roster-replacer.h"
#include "roster/roster.h"
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
	auto service = CurrentAccount.protocolHandler()->buddyListSerializationService();
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

		auto list = service->deserialize(stream);

		if (list.isEmpty())
		{
			MessageDialog::show(KaduIcon("dialog-error"), tr("Kadu"),
							tr("Contacts list couldn't be imported. File %0 doesn't contain correct contacts list.").arg(fileName));
			return;
		}

		auto result = Core::instance()->rosterReplacer()->replaceRoster(CurrentAccount, list, false);
		auto unImportedContacts = result.second;
		auto contactsList = QStringList{};
		for (auto &&contact : unImportedContacts)
			contactsList.append(contact.display(true) + " (" + contact.id() + ')');

		if (!unImportedContacts.isEmpty())
		{
			MessageDialog *dialog = MessageDialog::create(KaduIcon("dialog-question"),
				tr("Kadu"),
				tr("The following contacts from your list were not found in file:<br/><b>%1</b>.<br/>"
				"Do you want to remove them from contact list?").arg(contactsList.join("</b>, <b>")));
			dialog->addButton(QMessageBox::Yes, tr("Remove"));
			dialog->addButton(QMessageBox::No, tr("Cancel"));

			if (dialog->ask())
			{
				for (auto &&contact : unImportedContacts)
				{
					Buddy ownerBuddy = contact.ownerBuddy();
					contact.setOwnerBuddy(Buddy::null);
					// remove even if it still has some data, e.g. mobile number
					BuddyManager::instance()->removeBuddyIfEmpty(ownerBuddy, true);
					Roster::instance()->removeContact(contact);
				}
			}
		}
	}
}

void AccountBuddyListWidget::storeToFile()
{
	auto service = CurrentAccount.protocolHandler()->buddyListSerializationService();
	if (!service)
		return;

	QString fileName = QFileDialog::getSaveFileName(this, tr("Select file"), QString(), tr("Contact List Files (*.txt)"), 0);
	if (fileName.isEmpty())
		return;

	QFile file(fileName);

	if (file.open(QFile::WriteOnly))
	{
		file.write(service->serialize(BuddyManager::instance()->buddies(CurrentAccount)));
		file.close();
	}
}

#include "moc_account-buddy-list-widget.cpp"
