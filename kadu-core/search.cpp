/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "chat_manager.h"
#include "debug.h"
#include "kadu.h"
#include "message_box.h"
#include "misc.h"
#include "toolbar.h"
#include "userinfo.h"

#include "search.h"

SearchActionsSlots *SearchDialog::searchActionsSlot;

ActionDescription *SearchDialog::firstSearchAction;
ActionDescription *SearchDialog::nextResultsAction;
ActionDescription *SearchDialog::stopSearchAction;
ActionDescription *SearchDialog::clearResultsAction;
ActionDescription *SearchDialog::addFoundAction;
ActionDescription *SearchDialog::chatFoundAction;

SearchDialog::SearchDialog(QWidget *parent, UinType whoisSearchUin)
	: KaduMainWindow(parent),
	only_active(0), e_uin(0), e_name(0), e_nick(0), e_byrFrom(0), e_byrTo(0), e_surname(0),
	c_gender(0), e_city(0), results(0), progress(0), r_uin(0), r_pers(0), _whoisSearchUin(whoisSearchUin),
	seq(0), selectedUsers(new UserGroup(1)), searchRecord(new SearchRecord()), searchhidden(false), searching(false), workaround(false)
{
	kdebugf();

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Search user in directory"));

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

	l_gender = new QLabel(tr("Gender"), centralWidget);
	c_gender = new QComboBox(centralWidget);
	c_gender->insertItem(" ", 0);
	c_gender->insertItem(tr("Male"), 1);
	c_gender->insertItem(tr("Female"), 2);
	connect(c_gender, SIGNAL(activated(const QString &)), this, SLOT(personalDataTyped()));

	l_name = new QLabel(tr("Name"), centralWidget);
	e_name = new QLineEdit(centralWidget);
	connect(e_name, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_surname = new QLabel(tr("Surname"), centralWidget);
	e_surname = new QLineEdit(centralWidget);
	connect(e_surname, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_byr = new QLabel(tr("Birthyear"), centralWidget);
	l_byrFrom = new QLabel(tr("from"), centralWidget);
	e_byrFrom = new QLineEdit(centralWidget);
	e_byrFrom->setMaxLength(4);
	e_byrFrom->setValidator(new QIntValidator(1, 2100, this));
	connect(e_byrFrom, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_byrFrom, SIGNAL(textChanged(const QString &)), this, SLOT(byrFromDataTyped()));
	l_byrTo = new QLabel(tr("to"), centralWidget);
	e_byrTo = new QLineEdit(centralWidget);
	e_byrTo->setEnabled(false);
	e_byrTo->setMaxLength(4);
	e_byrTo->setValidator(new QIntValidator(1, 2100, centralWidget));
	connect(e_byrTo, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_city = new QLabel(tr("City"), centralWidget);
	e_city = new QLineEdit(centralWidget);
	connect(e_city, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	only_active = new QCheckBox(tr("Only active users"), centralWidget);
	connect(only_active, SIGNAL(clicked()), this, SLOT(personalDataTyped()));

	QGroupBox *qgrp1 = new QGroupBox(tr("Uin"), centralWidget);
	QHBoxLayout *uinLayout = new QHBoxLayout(qgrp1);
	l_uin = new QLabel(tr("Uin"),qgrp1);
	e_uin = new QLineEdit(qgrp1);
	e_uin->setMaxLength(8);
	e_uin->setValidator(new QIntValidator(1, 99999999, centralWidget));
	connect(e_uin, SIGNAL(textChanged(const QString &)), this, SLOT(uinTyped()));
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
	grid->addWidget(l_nick, 1, 0, Qt::AlignRight); grid->addWidget(e_nick, 1, 1);
	grid->addWidget(l_gender, 2, 0, Qt::AlignRight); grid->addWidget(c_gender, 2, 1);
	grid->addWidget(l_name, 1, 3, Qt::AlignRight); grid->addWidget(e_name, 1, 4);
	grid->addWidget(l_surname, 2, 3, Qt::AlignRight); grid->addWidget(e_surname, 2, 4);
	grid->addWidget(l_byr, 1, 6, Qt::AlignRight);
	grid->addWidget(l_byrFrom, 1, 7, Qt::AlignRight); grid->addWidget(e_byrFrom, 1, 8);
	grid->addWidget(l_byrTo, 2, 7, Qt::AlignRight); grid->addWidget(e_byrTo, 2, 8);
	grid->addWidget(l_city, 1, 10, Qt::AlignRight); grid->addWidget(e_city, 1, 11);

	grid->addMultiCellWidget(only_active, 2, 2, 10, 11);
	grid->addMultiCellWidget(qgrp1, 3, 3, 0, 3);
	grid->addMultiCellWidget(btngrp, 3, 3, 4, 11);
	grid->addMultiCellWidget(results, 5, 5, 0, 11);
	grid->addMultiCellWidget(progress, 6, 6, 0, 1);

	grid->addColSpacing(2, 10);
	grid->addColSpacing(5, 10);
	grid->addColSpacing(9, 10);

	grid->setResizeMode(QLayout::Minimum);
	
	QStringList headers;
	headers << tr("Status") << tr("Uin") << tr("Name") << tr("City") << tr("Nickname") << tr("Birth year");
	results->setHeaderLabels(headers);
	results->setAllColumnsShowFocus(true);
	results->setSelectionMode(QAbstractItemView::SingleSelection);
	results->setIndentation(false);
//	results->setResizeMode(Q3ListView::AllColumns);
//	for (int i = 1; i < 5; ++i)
//		results->setColumnWidthMode(i, Q3ListView::Maximum);

//	searchhidden = false;
	if (_whoisSearchUin)
		e_uin->insert(QString::number(_whoisSearchUin));

	setCentralWidget(centralWidget);

	if (loadToolBarsFromConfig("searchDockArea", Qt::BottomToolBarArea, true))
		writeToolBarsToConfig("search"); // port old config
	else
		loadToolBarsFromConfig("search"); // load new config

	loadGeometry(this, "General", "SearchDialogGeometry", 0, 30, 800, 350);

	connect(gadu, SIGNAL(newSearchResults(SearchResults &, int, int)), this, SLOT(newSearchResults(SearchResults &, int, int)));

	kdebugf2();
}

SearchDialog::~SearchDialog()
{
	kdebugf();

	writeToolBarsToConfig("search");

	disconnect(gadu, SIGNAL(newSearchResults(SearchResults&, int, int)), this, SLOT(newSearchResults(SearchResults&, int, int)));
// 	saveGeometry(this, "General", "SearchDialogGeometry");
	delete searchRecord;
	delete selectedUsers;
	kdebugf2();
}

void SearchDialog::initModule()
{
	kdebugf();

	searchActionsSlot = new SearchActionsSlots();

	firstSearchAction = new ActionDescription(
		ActionDescription::TypeSearch, "firstSearchAction",
		searchActionsSlot, SLOT(firstSearchActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("&Search")
	);

	nextResultsAction = new ActionDescription(
		ActionDescription::TypeSearch, "nextResultsAction",
		searchActionsSlot, SLOT(nextResultsActionActivated(QAction *, bool)),
		"NextSearchResults", tr("&Next results")
	);

	stopSearchAction = new ActionDescription(
		ActionDescription::TypeSearch, "stopSearchAction",
		searchActionsSlot, SLOT(stopSearchActionActivated(QAction *, bool)),
		"CloseWindow", tr("Stop")
	);

	clearResultsAction = new ActionDescription(
		ActionDescription::TypeSearch, "clearSearchAction",
		searchActionsSlot, SLOT(clearResultsActionActivated(QAction *, bool)),
		"ClearSearchResults", tr("Clear results")
	);

	addFoundAction = new ActionDescription(
		ActionDescription::TypeSearch, "addSearchedAction",
		searchActionsSlot, SLOT(addFoundActionActivated(QAction *, bool)),
		"AddUser", tr("Add selected user")
	);

	chatFoundAction = new ActionDescription(
		ActionDescription::TypeSearch, "chatSearchedAction",
		searchActionsSlot, SLOT(chatFoundActionActivated(QAction *, bool)),
		"OpenChat", tr("&Chat")
	);

	ToolBar::addDefaultAction("Search toolbar", "firstSearchAction", -1, true);
	ToolBar::addDefaultAction("Search toolbar", "nextResultsAction", 0, true);
	ToolBar::addDefaultAction("Search toolbar", "stopSearchAction", 1, true);
	ToolBar::addDefaultAction("Search toolbar", "clearSearchAction", 2, true);
	ToolBar::addDefaultAction("Search toolbar", "addSearchedAction", 3, true);
	ToolBar::addDefaultAction("Search toolbar", "chatSearchedAction", 4, true);

	kdebugf2();
}

void SearchDialog::closeModule()
{
	delete searchActionsSlot;

#if DEBUG_ENABLED
	// for valgrind
	QStringList searchActions;
	searchActions << "stopSearchAction" << "firstSearchAction" << "nextResultsAction" << "clearSearchAction" << "addSearchedAction" << "chatSearchedAction";
	CONST_FOREACH(act, searchActions)
	{
		ActionDescription *a = KaduActions[*act];
		delete a;
	}
#endif
}

QTreeWidgetItem * SearchDialog::selectedItem()
{
	if (results->selectedItems().count())
		return results->selectedItems()[0];
	else if (results->children().count() == 1)
		return dynamic_cast<QTreeWidgetItem *>(results->children()[0]);
	else 
		return NULL;
}

void SearchDialog::addFound()
{
	UserListElements found = selected();

	foreach (UserListElement user, found)
		if (user.isAnonymous())
			(new UserInfo(user, kadu))->show();
		else
			(new UserInfo(user, kadu))->show();
}

void SearchDialog::chatFound()
{
	UserListElements found = selected();
	if (found.size())
		chat_manager->openChatWidget(gadu, found);
}

// TODO: return real list
UserListElements SearchDialog::selected()
{
	UserListElements result;

	QTreeWidgetItem *selected = selectedItem();

	if (!selected)
		return result;

	QString uin = selected->text(1);
	QString firstname = selected->text(2);
	QString nickname = selected->text(4);

	bool ok;
	if (uin.toUInt(&ok) == 0 || !ok)
		return result;

	QString altnick;
	if (!nickname.isEmpty()) // Build altnick. Trying user nick first.
		altnick = nickname;
	else if (!firstname.isEmpty()) // If nick is empty, try firstname.
		altnick = firstname;
	else
		altnick = uin; // If above are empty, use uin.

	UserListElement e = userlist->byID("Gadu", uin);

	if (e.isAnonymous())
	{
		e.setFirstName(firstname);
		e.setNickName(nickname);
		e.setAltNick(altnick);
	}

	result.append(e);
	return result;
}

void SearchDialog::clearResults()
{
	results->clear();

// 	add_searched_action->setEnabled(this, false);
// 	clear_search_action->setEnabled(this, false);
// 	chat_searched_action->setEnabled(this, false);
}

void SearchDialog::stopSearch()
{
	kdebugf();

	gadu->stopSearchInPubdir(*searchRecord);

// 	stop_search_action->setEnabled(this, false);

// 	if ((r_pers->isChecked() && !isPersonalDataEmpty()) || (r_uin->isChecked() && !e_uin->text().isEmpty()))
// 		first_search_action->setEnabled(this, true);
	if (!results->selectedItems().isEmpty()) 
	{
// 		if (r_pers->isChecked() && !isPersonalDataEmpty())
// 			next_results_action->setEnabled(this, true);

// 		clear_search_action->setEnabled(this, true);
// 		add_searched_action->setEnabled(this, true);
// 		chat_searched_action->setEnabled(this, true);
	}

	kdebugf2();
}

void SearchDialog::firstSearch()
{
	kdebugf();

	if (gadu->currentStatus().isOffline())
	{
		MessageBox::msg(tr("Cannot search contacts in offline mode"), false, "Critical", this);
		kdebugf2();
		return;
	}

	if (!results->children().isEmpty())
		clearResults();

	searchRecord->clearData();

	if (r_pers->isChecked())
	{
		searchRecord->reqFirstName(e_name->text());
		searchRecord->reqLastName(e_surname->text());
		searchRecord->reqNickName(e_nick->text());
		searchRecord->reqCity(e_city->text());
		if (((e_byrTo->text().isEmpty()) && (!e_byrFrom->text().isEmpty()))
		    || ((e_byrTo->text().toUShort()) < (e_byrFrom->text().toUShort())))
			e_byrTo->setText(e_byrFrom->text());
		searchRecord->reqBirthYear(e_byrFrom->text(), e_byrTo->text());

		switch (c_gender->currentItem())
		{
			case 1:
				searchRecord->reqGender(false);
				break;
			case 2:
				searchRecord->reqGender(true);
				break;
		}

		if (only_active->isChecked())
			searchRecord->reqActive();
	}
	else if (r_uin->isChecked())
		searchRecord->reqUin(e_uin->text());

	searching = true;

// 	stop_search_action->setEnabled(this, true);
// 	first_search_action->setEnabled(this, false);
// 	next_results_action->setEnabled(this, false);
// 	add_searched_action->setEnabled(this, false);
// 	chat_searched_action->setEnabled(this, false);

	gadu->searchInPubdir(*searchRecord);

	progress->setText(tr("Searching..."));

	kdebugf2();
}

void SearchDialog::nextSearch()
{
	kdebugf();

	if (gadu->currentStatus().isOffline())
		return;

	searching = true;

// 	stop_search_action->setEnabled(this, true);
// 	first_search_action->setEnabled(this, false);
// 	next_results_action->setEnabled(this, false);
// 	add_searched_action->setEnabled(this, false);
// 	chat_searched_action->setEnabled(this, false);

	gadu->searchNextInPubdir(*searchRecord);

	progress->setText(tr("Searching..."));

	kdebugf2();
}

void SearchDialog::newSearchResults(SearchResults& searchResults, int seq, int fromUin)
{
	kdebugf();

	QTreeWidgetItem *qlv = 0;
	QPixmap pix;

	if ((seq != searchRecord->Seq) || searchRecord->IgnoreResults)
		return;

	searchRecord->FromUin = fromUin;

	// ??	if ((status && atoi(status) <= 1 && only_active->isChecked()) || !status)

	CONST_FOREACH(searchIterator, searchResults)
	{
		QList <QTreeWidgetItem *> items = results->findItems((*searchIterator).Uin, Qt::MatchExactly, 1);
		if (items.count())
			qlv = items[0];

		pix = ((*searchIterator).Stat).pixmap((*searchIterator).Stat.status(), false, false);

		if (qlv)
		{
//			if (!searchhidden) {
			qlv->setText(1, (*searchIterator).Uin);
			qlv->setText(2, (*searchIterator).First);
			qlv->setText(3, (*searchIterator).City);
			qlv->setText(4, (*searchIterator).Nick);
			qlv->setText(5, (*searchIterator).Born);
//	}
//			else
//				searchhidden = false;
		}
		else
		{
			QStringList strings;
			strings << QString::null << (*searchIterator).Uin << (*searchIterator).First <<(*searchIterator).City << (*searchIterator).Nick << (*searchIterator).Born;
			qlv = new QTreeWidgetItem(results, strings);
//			if (count == 1 && r_uin->isChecked() && !searchhidden
//				&& (statuscode == GG_STATUS_NOT_AVAIL || statuscode == GG_STATUS_NOT_AVAIL_DESCR)) {
//				qlv->setPixmap(0, pix);
//				searchhidden = true;
//				nextSearch();
//				return;
//				}
		//	}
			qlv->setIcon(0, QIcon(pix));
			qlv = 0;
		}
	}

	progress->setText(tr("Done searching"));

// 	if (!results->selectedItem())
// 		results->setSelected(results->firstChild(), true);

//	searchhidden = false;
// 	if ((r_pers->isChecked() && !isPersonalDataEmpty()) || (r_uin->isChecked() && !e_uin->text().isEmpty()))
// 		first_search_action->setEnabled(this, true);
// 	stop_search_action->setEnabled(this, false);

	if (searchResults.isEmpty())
	{
		kdebugmf(KDEBUG_INFO, "No results. Exit.\n");
		MessageBox::msg(tr("There were no results of your search"), false, "Information", this);
//		searchhidden = false;
	}
	else
	{
// 		if (r_pers->isChecked() && !isPersonalDataEmpty())
// 			next_results_action->setEnabled(this, true);

// 		clear_search_action->setEnabled(this, true);
// 		add_searched_action->setEnabled(this, true);
// 		chat_searched_action->setEnabled(this, true);
	}

	searching = false;

	kdebugf2();
}

void SearchDialog::closeEvent(QCloseEvent * e)
{
	QWidget::closeEvent(e);
}

void SearchDialog::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
//	results->triggerUpdate();//workaround for bug in Qt, which do not refresh results properly
}

void SearchDialog::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}

void SearchDialog::uinTyped(void)
{
	r_uin->setChecked(true);

// 	first_search_action->setEnabled(this, !e_uin->text().isEmpty());
}

void SearchDialog::personalDataTyped(void)
{
	workaround = true;
	r_pers->setChecked(true);
	workaround = false;

// 	first_search_action->setEnabled(this, !isPersonalDataEmpty());
// 	next_results_action->setEnabled(this, false);
}

void SearchDialog::byrFromDataTyped(void)
{
	bool b = e_byrFrom->text().isEmpty();
	e_byrTo->setEnabled(!b);
	if (b)
		e_byrTo->setText(QString::null);
}

void SearchDialog::persClicked()
{
// 	only_active->setEnabled(true);
// 	only_active->setChecked(false);
	if (!workaround)
		e_nick->setFocus();

// 	first_search_action->setEnabled(this, !isPersonalDataEmpty());
}

void SearchDialog::uinClicked()
{
// 	only_active->setEnabled(false);
	if (!workaround)
		e_uin->setFocus();

// 	first_search_action->setEnabled(this, !e_uin->text().isEmpty());
// 	next_results_action->setEnabled(this, false);
}

void SearchDialog::updateInfoClicked()
{
	kdebugf();

	QTreeWidgetItem *selected = selectedItem();

	if (!selected)
		return;

	QString suin = selected->text(1);
	QString firstname = selected->text(2);
//	QString lastname = selected->text(3);
	QString nickname = selected->text(4);

	UinType uin = suin.toUInt();
	UserListElement ule = userlist->byID("Gadu", QString::number(uin));

	// Build altnick. Try user nick first.
	QString altnick = nickname;
	// If nick is empty, try firstname+lastname.
	if (altnick.isEmpty())
	{
		altnick = firstname;
//		if (firstname.length() && lastname.length())
//			altnick += " ";
//		altnick += lastname;
	}
	// If nick is empty, use uin.
	if (altnick.isEmpty())
		altnick = uin;

	ule.setFirstName(firstname);
	ule.setNickName(nickname);
	(new UserInfo(ule, kadu))->show();
	kdebugf2();
}
/*
void SearchDialog::actionsAddedToToolbar(ToolButton* button, ToolBar* toolbar)
{
	kdebugf();
	if (!results->selectedItem())
		button->setEnabled(false);
	kdebugf2();
}

void SearchDialog::clearResultsActionAddedToToolbar(ToolButton* button, ToolBar* toolbar)
{
	kdebugf();
	if (!results->firstChild())
		button->setEnabled(false);
	kdebugf2();
}

void SearchDialog::stopSearchActionAddedToToolbar(ToolButton* button, ToolBar* toolbar)
{
	kdebugf();
	if (!searching)
		button->setEnabled(false);
	kdebugf2();
}

void SearchDialog::firstSearchActionAddedToToolbar(ToolButton* button, ToolBar* toolbar)
{
	kdebugf();
	if (searching || (r_pers->isChecked() && isPersonalDataEmpty()) || (r_uin->isChecked() && e_uin->text().isEmpty()))
		button->setEnabled(false);
	kdebugf2();
}

void SearchDialog::nextResultsActionAddedToToolbar(ToolButton* button, ToolBar* toolbar)
{
	kdebugf();
	if (searching || r_uin->isChecked() || isPersonalDataEmpty())
	{
		button->setEnabled(false);
		return;
	}

	clearResultsActionAddedToToolbar(button, toolbar);
	kdebugf2();
}*/

bool SearchDialog::isPersonalDataEmpty() const
{
	return
		e_name->text().isEmpty() &&
		e_nick->text().isEmpty() &&
		e_byrFrom->text().isEmpty() &&
		e_surname->text().isEmpty() &&
		c_gender->currentItem() == 0 &&
		e_city->text().isEmpty();
}

void SearchDialog::selectionChanged()
{
	//kdebugmf(KDEBUG_FUNCTION_START, "%p\n", );
// 	bool enableActions = results->selectedItem() != 0;
// 	add_searched_action->setEnabled(this, enableActions);
// 	chat_searched_action->setEnabled(this, enableActions);
}

void SearchActionsSlots::firstSearchActionActivated(QAction *sender, bool toggled)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(sender->parent());
	if (search)
		search->firstSearch();
}


// 	stop_search_action->setSlot(SLOT(stopSearch()));
// 	stop_search_action->setEnabled(this, false);
// 	first_search_action->setSlot(SLOT(firstSearch()));
// 	first_search_action->setEnabled(this, false);
// 	next_results_action->setSlot(SLOT(nextSearch()));
// 	next_results_action->setEnabled(this, false);
// 	clear_search_action->setSlot(SLOT(clearResults()));
// 	clear_search_action->setEnabled(this, false);
// 	add_searched_action->setEnabled(tl_uinhis, false);
// 	chat_searched_action->setEnabled(this, false);

void SearchActionsSlots::nextResultsActionActivated(QAction *sender, bool toggled)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(sender->parent());
	if (search)
		search->nextSearch();
}

void SearchActionsSlots::stopSearchActionActivated(QAction *sender, bool toggled)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(sender->parent());
	if (search)
		search->stopSearch();
}

void SearchActionsSlots::clearResultsActionActivated(QAction *sender, bool toggled)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(sender->parent());
	if (search)
		search->clearResults();
}

void SearchActionsSlots::addFoundActionActivated(QAction *sender, bool toggled)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(sender->parent());
	if (search)
		search->addFound();
}

void SearchActionsSlots::chatFoundActionActivated(QAction *sender, bool toggled)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(sender->parent());
	if (search)
		search->chatFound();
}
