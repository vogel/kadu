/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QAction>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>

#include "accounts/account-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "gui/widgets/toolbar.h"
#include "protocols/protocol.h"
#include "protocols/services/search-service.h"
#include "debug.h"

#include "search-window.h"
#include <protocols/protocol-factory.h>

SearchActionsSlots *SearchWindow::searchActionsSlot = 0;

ActionDescription *SearchWindow::firstSearchAction;
ActionDescription *SearchWindow::nextResultsAction;
ActionDescription *SearchWindow::stopSearchAction;
ActionDescription *SearchWindow::clearResultsAction;
ActionDescription *SearchWindow::addFoundAction;
ActionDescription *SearchWindow::chatFoundAction;

SearchWindow::SearchWindow(QWidget *parent, Buddy buddy) :
		MainWindow("search", parent),
		only_active(0), e_uin(0), e_name(0), e_nick(0), e_byrFrom(0), e_byrTo(0), e_surname(0),
		c_gender(0), e_city(0), results(0), progress(0), r_uin(0), r_pers(0),
		seq(0), selectedUsers(BuddySet()), CurrentSearchCriteria(BuddySearchCriteria()),
		searchhidden(false), searching(false), workaround(false)
{
	kdebugf();

	setWindowRole("kadu-search");

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Search user in directory"));

	initModule();

	QWidget *centralWidget = new QWidget(this);

	QLabel *l_name;
	QLabel *l_nick;
	QLabel *l_byr;
	QLabel *l_byrFrom;
	QLabel *l_byrTo;
	QLabel *l_surname;
	QLabel *l_gender;
	QLabel *l_city;
	QLabel *l_uin;

	l_nick = new QLabel(tr("Nickname"), centralWidget);
	e_nick = new QLineEdit(centralWidget);
	connect(e_nick, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_nick, SIGNAL(returnPressed()), this, SLOT(firstSearch()));

	l_gender = new QLabel(tr("Gender"), centralWidget);
	c_gender = new QComboBox(centralWidget);
	c_gender->insertItem(0, " ");
	c_gender->insertItem(1, tr("Male"));
	c_gender->insertItem(2, tr("Female"));
	connect(c_gender, SIGNAL(activated(const QString &)), this, SLOT(personalDataTyped()));

	l_name = new QLabel(tr("Name"), centralWidget);
	e_name = new QLineEdit(centralWidget);
	connect(e_name, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_name, SIGNAL(returnPressed()), this, SLOT(firstSearch()));

	l_surname = new QLabel(tr("Surname"), centralWidget);
	e_surname = new QLineEdit(centralWidget);
	connect(e_surname, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_surname, SIGNAL(returnPressed()), this, SLOT(firstSearch()));

	l_byr = new QLabel(tr("Birthyear"), centralWidget);
	l_byrFrom = new QLabel(tr("from"), centralWidget);
	e_byrFrom = new QLineEdit(centralWidget);
	e_byrFrom->setMaxLength(4);
	e_byrFrom->setValidator(new QIntValidator(1, 2100, this));
	connect(e_byrFrom, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_byrFrom, SIGNAL(textChanged(const QString &)), this, SLOT(byrFromDataTyped()));
	connect(e_byrFrom, SIGNAL(returnPressed()), this, SLOT(firstSearch()));
	l_byrTo = new QLabel(tr("to"), centralWidget);
	e_byrTo = new QLineEdit(centralWidget);
	e_byrTo->setEnabled(false);
	e_byrTo->setMaxLength(4);
	e_byrTo->setValidator(new QIntValidator(1, 2100, centralWidget));
	connect(e_byrTo, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_byrTo, SIGNAL(returnPressed()), this, SLOT(firstSearch()));

	l_city = new QLabel(tr("City"), centralWidget);
	e_city = new QLineEdit(centralWidget);
	connect(e_city, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_city, SIGNAL(returnPressed()), this, SLOT(firstSearch()));

	only_active = new QCheckBox(tr("Only active users"), centralWidget);
	connect(only_active, SIGNAL(clicked()), this, SLOT(personalDataTyped()));

	QGroupBox *qgrp1 = new QGroupBox(tr("Uin"), centralWidget);
	QHBoxLayout *uinLayout = new QHBoxLayout(qgrp1);
	l_uin = new QLabel(tr("Uin"),qgrp1);
	e_uin = new QLineEdit(qgrp1);
	e_uin->setMaxLength(9);
	e_uin->setValidator(new QIntValidator(1, 999999999, centralWidget));
	connect(e_uin, SIGNAL(textChanged(const QString &)), this, SLOT(uinTyped()));
	connect(e_uin, SIGNAL(returnPressed()), this, SLOT(firstSearch()));
	uinLayout->addWidget(l_uin);
	uinLayout->addWidget(e_uin);

	progress = new QLabel(centralWidget);

	results = new QTreeWidget(centralWidget);
	connect(results, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

	QGroupBox * btngrp = new QGroupBox(tr("Search criteria"), centralWidget);
	QButtonGroup *buttonGroup = new QButtonGroup(btngrp);
	QHBoxLayout *btngrpLayout = new QHBoxLayout(btngrp);
	r_pers = new QRadioButton(tr("&Personal data"), btngrp);
	r_pers->setChecked(true);
	connect(r_pers, SIGNAL(toggled(bool)), this, SLOT(persClicked()));
	r_pers->setToolTip(tr("Search using the personal data typed above (name, nickname)..."));

	r_uin = new QRadioButton(tr("&Uin number"), btngrp);
	connect(r_uin, SIGNAL(toggled(bool)), this, SLOT(uinClicked()));
	r_uin->setToolTip(tr("Search for this UIN exclusively"));

	buttonGroup->addButton(r_pers);
	buttonGroup->addButton(r_uin);

	btngrpLayout->addWidget(r_pers);
	btngrpLayout->addWidget(r_uin);

	QGridLayout * grid = new QGridLayout(centralWidget);

	grid->addWidget(l_nick, 1, 0, Qt::AlignRight);
	grid->addWidget(e_nick, 1, 1);
	grid->addWidget(l_name, 1, 3, Qt::AlignRight);
	grid->addWidget(e_name, 1, 4);
	grid->addWidget(l_byr, 1, 6, Qt::AlignRight);
	grid->addWidget(l_byrFrom, 1, 7, Qt::AlignRight);
	grid->addWidget(e_byrFrom, 1, 8);
	grid->addWidget(l_city, 1, 10, Qt::AlignRight);
	grid->addWidget(e_city, 1, 11);

	grid->addWidget(l_gender, 2, 0, Qt::AlignRight);
	grid->addWidget(c_gender, 2, 1);
	grid->addWidget(l_surname, 2, 3, Qt::AlignRight);
	grid->addWidget(e_surname, 2, 4);
	grid->addWidget(l_byrTo, 2, 7, Qt::AlignRight);
	grid->addWidget(e_byrTo, 2, 8);
	grid->addWidget(only_active, 2, 10, 1, 2);

	grid->addWidget(qgrp1, 3, 0, 1, 4);
	grid->addWidget(btngrp, 3, 4, 1, 8);
	grid->addWidget(results, 5, 0, 1, 12);
	grid->addWidget(progress, 6, 0, 1, 2);

	QStringList headers;
	headers << tr("Status") << tr("Uin") << tr("Name") << tr("City") << tr("Nickname") << tr("Birth year");
	results->setHeaderLabels(headers);
	results->setSortingEnabled(true);
	results->setAllColumnsShowFocus(true);
	results->setSelectionMode(QAbstractItemView::SingleSelection);
	results->setIndentation(false);

	setCentralWidget(centralWidget);

	if (loadOldToolBarsFromConfig("searchDockArea", Qt::BottomToolBarArea))
		writeToolBarsToConfig(); // port old config
	else
		loadToolBarsFromConfig(); // load new config

 	setActionState(stopSearchAction, false);
 	setActionState(firstSearchAction, false);
 	setActionState(nextResultsAction,false);
 	setActionState(clearResultsAction, false);
 	setActionState(addFoundAction, false);
 	setActionState(chatFoundAction, false);

	loadWindowGeometry(this, "General", "SearchWindowGeometry", 0, 50, 800, 350);

	if (buddy)
	{
		CurrentAccount = BuddyPreferredManager::instance()->preferredAccount(buddy);
	}
	else
	{
		foreach (const Account &a, AccountManager::instance()->items())
		{
			// TODO 0.6.6: !!!
			if (a.protocolHandler()->isConnected() && a.protocolHandler()->protocolFactory()->name() == "gadu")
			{
				CurrentAccount = a;
				break;
			}
		}
		if (CurrentAccount.isNull())
			CurrentAccount = AccountManager::instance()->items().at(0);
	}

	if (CurrentAccount.protocolHandler())
		CurrentSearchService = CurrentAccount.protocolHandler()->searchService();

	if (CurrentSearchService)
		connect(CurrentSearchService, SIGNAL(newResults(BuddyList)), this, SLOT(newSearchResults(BuddyList)));

	if (!buddy.isNull())
	{
		CurrentSearchCriteria.SearchBuddy = buddy;
		QList<Contact> contactslist = buddy.contacts(CurrentAccount);
		Contact contact = contactslist.isEmpty() ? Contact::null : contactslist.at(0);
		e_uin->insert(contact.id());
	}

	kdebugf2();
}

SearchWindow::~SearchWindow()
{
	kdebugf();
 	saveWindowGeometry(this, "General", "SearchWindowGeometry");
	closeModule();
	kdebugf2();
}

void SearchWindow::initModule()
{
	kdebugf();

	searchActionsSlot = new SearchActionsSlots();

	firstSearchAction = new ActionDescription(searchActionsSlot,
		ActionDescription::TypeSearch, "firstSearchAction",
		searchActionsSlot, SLOT(firstSearchActionActivated(QAction *, bool)),
		"edit-find", tr("&Search")
	);
	QObject::connect(firstSearchAction, SIGNAL(actionCreated(Action *)), searchActionsSlot, SLOT(firstSearchActionCreated(Action *)));

	nextResultsAction = new ActionDescription(searchActionsSlot,
		ActionDescription::TypeSearch, "nextResultsAction",
		searchActionsSlot, SLOT(nextResultsActionActivated(QAction *, bool)),
		"go-next", tr("&Next results")
	);
	QObject::connect(nextResultsAction, SIGNAL(actionCreated(Action *)), searchActionsSlot, SLOT(nextResultsActionCreated(Action *)));

	stopSearchAction = new ActionDescription(searchActionsSlot,
		ActionDescription::TypeSearch, "stopSearchAction",
		searchActionsSlot, SLOT(stopSearchActionActivated(QAction *, bool)),
		"dialog-cancel", tr("Stop")
	);
	QObject::connect(stopSearchAction, SIGNAL(actionCreated(Action *)), searchActionsSlot, SLOT(stopSearchActionCreated(Action *)));

	clearResultsAction = new ActionDescription(searchActionsSlot,
		ActionDescription::TypeSearch, "clearSearchAction",
		searchActionsSlot, SLOT(clearResultsActionActivated(QAction *, bool)),
		"edit-clear", tr("Clear results")
	);
	QObject::connect(clearResultsAction, SIGNAL(actionCreated(Action *)), searchActionsSlot, SLOT(clearResultsActionCreated(Action *)));

	addFoundAction = new ActionDescription(searchActionsSlot,
		ActionDescription::TypeSearch, "addSearchedAction",
		searchActionsSlot, SLOT(addFoundActionActivated(QAction *, bool)),
		"contact-new", tr("Add selected user")
	);
	QObject::connect(addFoundAction, SIGNAL(actionCreated(Action *)), searchActionsSlot, SLOT(actionsFoundActionCreated(Action *)));

	chatFoundAction = new ActionDescription(searchActionsSlot,
		ActionDescription::TypeSearch, "chatSearchedAction",
		searchActionsSlot, SLOT(chatFoundActionActivated(QAction *, bool)),
		"internet-group-chat", tr("&Chat")
	);
	QObject::connect(chatFoundAction, SIGNAL(actionCreated(Action *)), searchActionsSlot, SLOT(actionsFoundActionCreated(Action *)));

	kdebugf2();
}

void SearchWindow::closeModule()
{
	delete searchActionsSlot;
	searchActionsSlot = 0;
}

QTreeWidgetItem * SearchWindow::selectedItem()
{
	if (results->selectedItems().count())
		return results->selectedItems().at(0);
	else if (results->children().count() == 1)
		return dynamic_cast<QTreeWidgetItem *>(results->children().at(0));
	else
		return NULL;
}

void SearchWindow::addFound()
{
	foreach (const Buddy &buddy, selected().toBuddySet().toList())
		(new AddBuddyWindow(this, buddy))->show();
}

void SearchWindow::chatFound()
{
	ContactSet contacts = selected();
	if (contacts.count() > 0)
	{
		Chat chat = ChatManager::instance()->findChat(contacts, true);
		if (chat)
			ChatWidgetManager::instance()->openPendingMessages(chat, true);
	}

}

// TODO: return real list
ContactSet SearchWindow::selected()
{
	ContactSet result;

	QTreeWidgetItem *selected = selectedItem();

	if (!selected)
		return result;

	QString uin = selected->text(1);
	QString firstname = selected->text(2);
	QString nickname = selected->text(4);

	QString altnick;
	if (!nickname.isEmpty()) // Build altnick. Trying user nick first.
		altnick = nickname;
	else if (!firstname.isEmpty()) // If nick is empty, try firstname.
		altnick = firstname;
	else
		altnick = uin; // If above are empty, use uin.

	Contact contact = ContactManager::instance()->byId(CurrentAccount, uin, ActionCreateAndAdd);
	Buddy e = contact.ownerBuddy();

	if (e.isAnonymous())
	{
		e.setFirstName(firstname);
		e.setNickName(nickname);
		e.setDisplay(altnick);
	}

	result.insert(contact);
	return result;
}

void SearchWindow::clearResults()
{
	results->clear();

 	setActionState(addFoundAction, false);
 	setActionState(clearResultsAction, false);
 	setActionState(chatFoundAction, false);
}

void SearchWindow::stopSearch()
{
	kdebugf();

	CurrentSearchService->stop();

	setActionState(stopSearchAction, false);

	if ((r_pers->isChecked() && !isPersonalDataEmpty()) || (r_uin->isChecked() && !e_uin->text().isEmpty()))
		setActionState(firstSearchAction, true);
	if (!results->selectedItems().isEmpty())
	{
		if (r_pers->isChecked() && !isPersonalDataEmpty())
			setActionState(nextResultsAction, true);

		setActionState(addFoundAction, true);
		setActionState(chatFoundAction, true);
	}
	if (results->topLevelItemCount() > 0)
		setActionState(clearResultsAction, true);

	kdebugf2();
}

void SearchWindow::firstSearch()
{
	kdebugf();

	if (r_pers->isChecked() && isPersonalDataEmpty())
		return;

	if (!CurrentSearchService)
	{
		MessageDialog::show("dialog-error", tr("Kadu"), tr("We don't offer contacts search feature yet for this network"), QMessageBox::Ok, this);
		kdebugf2();
		return;
	}

	if (!CurrentAccount.protocolHandler()->isConnected())
	{
		MessageDialog::show("dialog-error", tr("Kadu"), tr("Cannot search contacts in offline mode"), QMessageBox::Ok, this);
		kdebugf2();
		return;
	}

	if (!results->children().isEmpty())
		clearResults();

	if (searching)
		CurrentSearchService->stop();

	CurrentSearchCriteria.clearData();

	if (r_pers->isChecked())
	{
		CurrentSearchCriteria.reqFirstName(e_name->text());
		CurrentSearchCriteria.reqLastName(e_surname->text());
		CurrentSearchCriteria.reqNickName(e_nick->text());
		CurrentSearchCriteria.reqCity(e_city->text());
		if (((e_byrTo->text().isEmpty()) && (!e_byrFrom->text().isEmpty()))
		    || ((e_byrTo->text().toUShort()) < (e_byrFrom->text().toUShort())))
			e_byrTo->setText(e_byrFrom->text());
		CurrentSearchCriteria.reqBirthYear(e_byrFrom->text(), e_byrTo->text());

		switch (c_gender->currentIndex())
		{
			case 1:
				CurrentSearchCriteria.reqGender(false);
				break;
			case 2:
				CurrentSearchCriteria.reqGender(true);
				break;
		}

		if (only_active->isChecked())
			CurrentSearchCriteria.reqActive();
	}
	else if (r_uin->isChecked())
		CurrentSearchCriteria.reqUin(CurrentAccount, e_uin->text());

	searching = true;

 	setActionState(stopSearchAction, true);
 	setActionState(firstSearchAction, false);
 	setActionState(nextResultsAction, false);
 	setActionState(addFoundAction, false);
 	setActionState(chatFoundAction, false);

	CurrentSearchService->searchFirst(CurrentSearchCriteria);

	progress->setText(tr("Searching..."));

	kdebugf2();
}

void SearchWindow::nextSearch()
{
	kdebugf();

	if (!CurrentAccount.protocolHandler()->isConnected())
		return;

	searching = true;

 	setActionState(stopSearchAction, true);
 	setActionState(firstSearchAction, false);
 	setActionState(nextResultsAction, false);
 	setActionState(addFoundAction, false);
	setActionState(chatFoundAction, false);

	CurrentSearchService->searchNext();

	progress->setText(tr("Searching..."));

	kdebugf2();
}

void SearchWindow::newSearchResults(const BuddyList &buddies)
{
	kdebugf();

	QTreeWidgetItem *qlv = 0;
	QIcon pix;

	foreach (const Buddy &buddy, buddies)
	{
		QList<Contact> contactslist = buddy.contacts(CurrentAccount);
		Contact contact = contactslist.isEmpty() ? Contact::null : contactslist.at(0);
		QList <QTreeWidgetItem *> items = results->findItems(contact.id(), Qt::MatchExactly, 1);
		if (items.count())
			qlv = items.at(0);
		pix = contact.contactAccount().data()->statusIcon(contact.currentStatus()).pixmap(16, 16);

		if (qlv)
		{
			qlv->setText(1, contact.id());
			qlv->setText(2, buddy.firstName());
			qlv->setText(3, buddy.city());
			qlv->setText(4, buddy.nickName());
			qlv->setText(5, buddy.familyCity());
		}
		else
		{
			QStringList strings;
			strings << QString() << contact.id() << buddy.firstName() << buddy.city() << buddy.nickName() << QString::number(buddy.birthYear());
			qlv = new QTreeWidgetItem(results, strings);
			qlv->setIcon(0, pix);
			qlv = 0;
		}
	}

	progress->setText(tr("Done searching"));

	if ((r_pers->isChecked() && !isPersonalDataEmpty()) || (r_uin->isChecked() && !e_uin->text().isEmpty()))
		setActionState(firstSearchAction, true);
	setActionState(stopSearchAction, false);

	if (buddies.isEmpty())
	{
		kdebugmf(KDEBUG_INFO, "No results. Exit.\n");
		MessageDialog::show("dialog-information", tr("Kadu"), tr("There were no results of your search"), QMessageBox::Ok, this);
	}
	else
	{
		if (r_pers->isChecked() && !isPersonalDataEmpty())
			setActionState(nextResultsAction, true);

		if (results->topLevelItemCount() > 0)
			setActionState(clearResultsAction, true);
	}

	if (!results->selectedItems().isEmpty())
	{
		setActionState(addFoundAction, true);
		setActionState(chatFoundAction, true);
	}

	searching = false;

	kdebugf2();
}

void SearchWindow::closeEvent(QCloseEvent * e)
{
	QWidget::closeEvent(e);
}

void SearchWindow::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
//	results->triggerUpdate();//workaround for bug in Qt, which do not refresh results properly
}

void SearchWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}

void SearchWindow::uinTyped(void)
{
	r_uin->setChecked(true);

 	setActionState(firstSearchAction, !e_uin->text().isEmpty());
}

void SearchWindow::personalDataTyped(void)
{
	workaround = true;
	r_pers->setChecked(true);
	workaround = false;

 	setActionState(firstSearchAction, !isPersonalDataEmpty());
 	setActionState(nextResultsAction, false);
}

void SearchWindow::byrFromDataTyped(void)
{
	bool b = e_byrFrom->text().isEmpty();
	e_byrTo->setEnabled(!b);
	if (b)
		e_byrTo->clear();
}

void SearchWindow::persClicked()
{
 	only_active->setEnabled(true);
 	only_active->setChecked(false);
	if (!workaround)
		e_nick->setFocus();

 	setActionState(firstSearchAction, !isPersonalDataEmpty());
}

void SearchWindow::uinClicked()
{
 	only_active->setEnabled(false);
	if (!workaround)
		e_uin->setFocus();

 	setActionState(firstSearchAction, !e_uin->text().isEmpty());
 	setActionState(nextResultsAction, false);
}

bool SearchWindow::isPersonalDataEmpty() const
{
	return
		e_name->text().isEmpty() &&
		e_nick->text().isEmpty() &&
		e_byrFrom->text().isEmpty() &&
		e_surname->text().isEmpty() &&
		c_gender->currentIndex() == 0 &&
		e_city->text().isEmpty();
}

void SearchWindow::selectionChanged()
{
	//kdebugmf(KDEBUG_FUNCTION_START, "%p\n", );
 	bool enableActions = results->selectedItems().count() > 0;
 	setActionState(addFoundAction, enableActions);
 	setActionState(chatFoundAction, enableActions);
}

void SearchActionsSlots::firstSearchActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	SearchWindow *search = dynamic_cast<SearchWindow *>(sender->parent());
	if (search)
		search->firstSearch();
}

void SearchWindow::createDefaultToolbars(QDomElement toolbarsConfig)
{
	QDomElement dockAreaConfig = getDockAreaConfigElement(toolbarsConfig, "search_bottomDockArea");
	QDomElement toolbarConfig = xml_config_file->createElement(dockAreaConfig, "ToolBar");

	addToolButton(toolbarConfig, "firstSearchAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "nextResultsAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "stopSearchAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "clearSearchAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "addSearchedAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "chatSearchedAction", Qt::ToolButtonTextUnderIcon);
}

void SearchWindow::setActionState(ActionDescription *actionDescription, bool toogle)
{
	Action *action = actionDescription->action(this);
	if (action)
		action->setEnabled(toogle);
}

void SearchActionsSlots::firstSearchActionCreated(Action *action)
{
	SearchWindow *search = dynamic_cast<SearchWindow *>(action->parent());

	if (!search || !search->r_pers || search->searching || (search->r_pers->isChecked() && search->isPersonalDataEmpty()) || (search->r_uin->isChecked() && search->e_uin->text().isEmpty()))
		action->setEnabled(false);
}

void SearchActionsSlots::nextResultsActionCreated(Action *action)
{
	SearchWindow *search = dynamic_cast<SearchWindow *>(action->parent());

	if (!search || !search->r_uin || search->searching || search->r_uin->isChecked() || search->isPersonalDataEmpty())
		action->setEnabled(false);

}

void SearchActionsSlots::stopSearchActionCreated(Action *action)
{
	SearchWindow *search = dynamic_cast<SearchWindow *>(action->parent());

	if (!search || !search->searching)
		action->setEnabled(false);
}

void SearchActionsSlots::clearResultsActionCreated(Action *action)
{
	SearchWindow *search = dynamic_cast<SearchWindow *>(action->parent());
	if (!search || !search->results || !search->results->topLevelItemCount())
		action->setEnabled(false);
}

void SearchActionsSlots::actionsFoundActionCreated(Action *action)
{
	SearchWindow *search = dynamic_cast<SearchWindow *>(action->parent());

	if (!search || !search->results || search->results->selectedItems().count() == 0)
		action->setEnabled(false);
}

void SearchActionsSlots::nextResultsActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	SearchWindow *search = dynamic_cast<SearchWindow *>(sender->parent());
	if (search)
		search->nextSearch();
}

void SearchActionsSlots::stopSearchActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	SearchWindow *search = dynamic_cast<SearchWindow *>(sender->parent());
	if (search)
		search->stopSearch();
}

void SearchActionsSlots::clearResultsActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	SearchWindow *search = dynamic_cast<SearchWindow *>(sender->parent());
	if (search)
		search->clearResults();
}

void SearchActionsSlots::addFoundActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	SearchWindow *search = dynamic_cast<SearchWindow *>(sender->parent());
	if (search)
		search->addFound();
}

void SearchActionsSlots::chatFoundActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	SearchWindow *search = dynamic_cast<SearchWindow *>(sender->parent());
	if (search)
		search->chatFound();
}
