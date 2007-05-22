/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qhbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtooltip.h>
#include <qvalidator.h>

#include "action.h"
#include "chat_manager.h"
#include "config_file.h"
#include "debug.h"
#include "dockarea.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc.h"
#include "search.h"
#include "userinfo.h"

SearchDialog::SearchDialog(QWidget *parent, const char *name, UinType whoisSearchUin)
	: QDialog (parent, name, FALSE, Qt::WDestructiveClose),
	only_active(0), e_uin(0), e_name(0), e_nick(0), e_byrFrom(0), e_byrTo(0), e_surname(0),
	c_gender(0), e_city(0), results(0), progress(0), r_uin(0), r_pers(0), _whoisSearchUin(whoisSearchUin),
	seq(0), selectedUsers(new UserGroup(1)), searchRecord(new SearchRecord()), searchhidden(false)
{
	kdebugf();

	QLabel *l_name;
	QLabel *l_nick;
	QLabel *l_byr;
	QLabel *l_byrFrom;
	QLabel *l_byrTo;
	QLabel *l_surname;
	QLabel *l_gender;
	QLabel *l_city;
	QLabel *l_uin;

	l_nick = new QLabel(tr("Nickname"),this);
	e_nick = new QLineEdit(this);
	connect(e_nick, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_gender = new QLabel(tr("Gender"),this);
	c_gender = new QComboBox(this);
	c_gender->insertItem(" ", 0);
	c_gender->insertItem(tr("Male"), 1);
	c_gender->insertItem(tr("Female"), 2);
	connect(c_gender, SIGNAL(activated(const QString &)), this, SLOT(personalDataTyped()));

	l_name = new QLabel(tr("Name"),this);
	e_name = new QLineEdit(this);
	connect(e_name, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_surname = new QLabel(tr("Surname"),this);
	e_surname = new QLineEdit(this);
	connect(e_surname, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_byr = new QLabel(tr("Birthyear"),this);
	l_byrFrom = new QLabel(tr("from"),this);
	e_byrFrom = new QLineEdit(this);
	e_byrFrom->setMaxLength(4);
	e_byrFrom->setValidator(new QIntValidator(1, 2100, this));
	connect(e_byrFrom, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_byrFrom, SIGNAL(textChanged(const QString &)), this, SLOT(byrFromDataTyped()));
	l_byrTo = new QLabel(tr("to"),this);
	e_byrTo = new QLineEdit(this);
	e_byrTo->setEnabled(false);
	e_byrTo->setMaxLength(4);
	e_byrTo->setValidator(new QIntValidator(1, 2100, this));
	connect(e_byrTo, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_city = new QLabel(tr("City"),this);
	e_city = new QLineEdit(this);
	connect(e_city, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	only_active = new QCheckBox(tr("Only active users"),this);
	connect(only_active, SIGNAL(clicked()), this, SLOT(personalDataTyped()));

	QGroupBox * qgrp1 = new QGroupBox(2, Qt::Horizontal, tr("Uin"), this);
	l_uin = new QLabel(tr("Uin"),qgrp1);
	e_uin = new QLineEdit(qgrp1);
	e_uin->setMaxLength(8);
	e_uin->setValidator(new QIntValidator(1, 99999999, this));
	connect(e_uin, SIGNAL(textChanged(const QString &)), this, SLOT(uinTyped()));

	progress = new QLabel(this);

	results = new QListView(this);
	connect(results, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

	QHButtonGroup * btngrp = new QHButtonGroup(tr("Search criteria"), this);
	r_pers = new QRadioButton(tr("&Personal data"),btngrp);
	r_pers->setChecked(true);
	connect(r_pers, SIGNAL(toggled(bool)), this, SLOT(persClicked()));
	QToolTip::add(r_pers, tr("Search using the personal data typed above (name, nickname)..."));

	r_uin = new QRadioButton(tr("&Uin number"),btngrp);
	connect(r_uin, SIGNAL(toggled(bool)), this, SLOT(uinClicked()));
	QToolTip::add(r_uin, tr("Search for this UIN exclusively"));

	DockArea* dock_area = new DockArea(Qt::Horizontal, DockArea::Normal, this,
		"searchDockArea", Action::TypeUserList | Action::TypeSearch);
	connect(dock_area, SIGNAL(selectedUsersNeeded(const UserGroup*&)),
		this, SLOT(selectedUsersNeeded(const UserGroup*&)));
	if (!dock_area->loadFromConfig(this))
	{
		ToolBar* toolbar = new ToolBar(this, "Search toolbar");
		toolbar->setOffset(1000);
		dock_area->moveDockWindow(toolbar);
		dock_area->setAcceptDockWindow(toolbar, true);
		KaduActions.addDefaultActionsToToolbar(toolbar);
	}

	QGridLayout * grid = new QGridLayout (this, 7, 12, 7, 5);
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
	grid->addMultiCellWidget(dock_area, 6, 6, 0, 11);
	grid->addMultiCellWidget(progress, 6, 6, 0, 1);

	grid->addColSpacing(2, 10);
	grid->addColSpacing(5, 10);
	grid->addColSpacing(9, 10);

	grid->setResizeMode(QLayout::Minimum);

	results->addColumn(tr("Status"));
	results->addColumn(tr("Uin"));
	results->addColumn(tr("Name"));
	results->addColumn(tr("City"));
	results->addColumn(tr("Nickname"));
	results->addColumn(tr("Birth year"));
	results->setAllColumnsShowFocus(true);
	results->setResizeMode(QListView::AllColumns);
	for (int i = 1; i < 5; ++i)
		results->setColumnWidthMode(i, QListView::Maximum);

	connect(KaduActions["addSearchedAction"], SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(addSearchedActionActivated(const UserGroup*)));
	connect(KaduActions["addSearchedAction"], SIGNAL(addedToToolbar(ToolButton*, ToolBar*)),
		this, SLOT(actionsAddedToToolbar(ToolButton*, ToolBar*)));
	connect(KaduActions["clearSearchAction"], SIGNAL(addedToToolbar(ToolButton*, ToolBar*)),
		this, SLOT(actionsAddedToToolbar(ToolButton*, ToolBar*)));
	connect(KaduActions["stopSearchAction"], SIGNAL(addedToToolbar(ToolButton*, ToolBar*)),
		this, SLOT(stopSearchActionAddedToToolbar(ToolButton*, ToolBar*)));
	connect(KaduActions["firstSearchAction"], SIGNAL(addedToToolbar(ToolButton*, ToolBar*)),
		this, SLOT(firstSearchActionAddedToToolbar(ToolButton*, ToolBar*)));
	connect(KaduActions["nextResultsAction"], SIGNAL(addedToToolbar(ToolButton*, ToolBar*)),
		this, SLOT(nextResultsActionAddedToToolbar(ToolButton*, ToolBar*)));

	KaduActions["stopSearchAction"]->setEnabled(this, false);
	KaduActions["firstSearchAction"]->setEnabled(this, false);
	KaduActions["nextResultsAction"]->setEnabled(this, false);
	KaduActions["clearSearchAction"]->setEnabled(this, false);
	KaduActions["addSearchedAction"]->setEnabled(this, false);

	connect(KaduActions["chatAction"], SIGNAL(addedToToolbar(ToolButton*, ToolBar*)),
		this, SLOT(actionsAddedToToolbar(ToolButton*, ToolBar*)));
	KaduActions["chatAction"]->setEnabled(this, false);

//	searchhidden = false;
	if (_whoisSearchUin)
	{
		r_uin->setChecked(true);
		e_uin->setText(QString::number(_whoisSearchUin));
	}

	loadGeometry(this, "General", "SearchDialogGeometry", 0, 30, 800, 350);
	setCaption(tr("Search user in directory"));

	connect(gadu, SIGNAL(newSearchResults(SearchResults &, int, int)), this, SLOT(newSearchResults(SearchResults &, int, int)));
	kdebugf2();
}

SearchDialog::~SearchDialog()
{
	kdebugf();
	disconnect(gadu, SIGNAL(newSearchResults(SearchResults&, int, int)), this, SLOT(newSearchResults(SearchResults&, int, int)));
	saveGeometry(this, "General", "SearchDialogGeometry");
	delete searchRecord;
	delete selectedUsers;
	kdebugf2();
}

void SearchDialog::initModule()
{
	kdebugf();

	Action* first_search_action = new Action(icons_manager->loadIcon("LookupUserInfo"),
		tr("&Search"), "firstSearchAction", Action::TypeSearch, Key_Return);
	first_search_action->setSlot(SLOT(firstSearch()));
	KaduActions.insert("firstSearchAction", first_search_action);
	KaduActions.addDefaultToolbarAction("Search toolbar", "firstSearchAction", 0, true);

	Action* next_results_action = new Action(icons_manager->loadIcon("NextSearchResults"),
		tr("&Next results"), "nextResultsAction", Action::TypeSearch);
	next_results_action->setSlot(SLOT(nextSearch()));
	KaduActions.insert("nextResultsAction", next_results_action);
	KaduActions.addDefaultToolbarAction("Search toolbar", "nextResultsAction", 1, true);

	Action* stop_search_action = new Action(icons_manager->loadIcon("CloseWindow"),
		tr("Stop"), "stopSearchAction", Action::TypeSearch);
	stop_search_action->setSlot(SLOT(stopSearch()));
	KaduActions.insert("stopSearchAction", stop_search_action);
	KaduActions.addDefaultToolbarAction("Search toolbar", "stopSearchAction", 2, true);

	Action* clear_search_action = new Action(icons_manager->loadIcon("ClearSearchResults"),
		tr("Clear results"), "clearSearchAction", Action::TypeSearch);
	clear_search_action->setSlot(SLOT(clearResults()));
	KaduActions.insert("clearSearchAction", clear_search_action);
	KaduActions.addDefaultToolbarAction("Search toolbar", "clearSearchAction", 3, true);

	Action* add_searched_action = new Action(icons_manager->loadIcon("AddUser"),
		tr("Add selected user"), "addSearchedAction", Action::TypeSearch);
	KaduActions.insert("addSearchedAction", add_searched_action);
	KaduActions.addDefaultToolbarAction("Search toolbar", "addSearchedAction", 4, true);

	kdebugf2();
}

void SearchDialog::closeModule()
{
#if DEBUG_ENABLED
	// for valgrind
	QStringList searchActions;
	searchActions << "stopSearchAction" << "firstSearchAction" << "nextResultsAction" << "clearSearchAction" << "addSearchedAction";
	CONST_FOREACH(act, searchActions)
	{
		Action *a = KaduActions[*act];
		KaduActions.remove(*act);
		delete a;
	}
#endif
}

void SearchDialog::selectedUsersNeeded(const UserGroup*& user_group)
{
	kdebugf();

	QListViewItem *selected = results->selectedItem();
	if (!selected)
	{
		if (results->childCount() == 1)
			selected = results->firstChild();
		user_group = NULL;
		return;
	}

	QString uin = selected->text(1);
	QString firstname = selected->text(2);
	QString nickname = selected->text(4);

	bool ok;
	if (uin.toUInt(&ok) == 0 || !ok)
		return;

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

	selectedUsers->clear();
	selectedUsers->addUser(e);
	user_group = selectedUsers;

	kdebugf2();
}

void SearchDialog::clearResults(void)
{
	results->clear();
	KaduActions["addSearchedAction"]->setEnabled(this, false);
	KaduActions["clearSearchAction"]->setEnabled(this, false);
	KaduActions["chatAction"]->setEnabled(this, false);
}

void SearchDialog::addSearchedActionActivated(const UserGroup* users)
{
	kdebugf();
	if ((*users->begin()).isAnonymous())
		(new UserInfo(*users->begin(), 0, "add user"))->show();
	else
		(new UserInfo(*users->begin(), 0, "user info"))->show();
	kdebugf2();
}

void SearchDialog::stopSearch(void)
{
	kdebugf();
	gadu->stopSearchInPubdir(*searchRecord);
	KaduActions["stopSearchAction"]->setEnabled(this, false);
	if ((r_pers->isChecked() && !isPersonalDataEmpty()) || (r_uin->isChecked() && !e_uin->text().isEmpty()))
		KaduActions["firstSearchAction"]->setEnabled(this, true);
	if (results->selectedItem()) {
		if (r_pers->isChecked() && !isPersonalDataEmpty())
			KaduActions["nextResultsAction"]->setEnabled(this, true);

		KaduActions["clearSearchAction"]->setEnabled(this, true);
		KaduActions["addSearchedAction"]->setEnabled(this, true);
		KaduActions["chatAction"]->setEnabled(this, true);
	}
	kdebugf2();
}

void SearchDialog::firstSearch(void)
{
	kdebugf();
	if (results->childCount())
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

	gadu->searchInPubdir(*searchRecord);

	KaduActions["stopSearchAction"]->setEnabled(this, true);
	KaduActions["firstSearchAction"]->setEnabled(this, false);
	KaduActions["nextResultsAction"]->setEnabled(this, false);
	KaduActions["addSearchedAction"]->setEnabled(this, false);
	KaduActions["chatAction"]->setEnabled(this, false);

	progress->setText(tr("Searching..."));
	kdebugf2();
}

void SearchDialog::nextSearch(void)
{
	if (gadu->currentStatus().isOffline())
		return;
	kdebugf();

	KaduActions["stopSearchAction"]->setEnabled(this, true);
	KaduActions["firstSearchAction"]->setEnabled(this, false);
	KaduActions["nextResultsAction"]->setEnabled(this, false);
	KaduActions["addSearchedAction"]->setEnabled(this, false);
	KaduActions["chatAction"]->setEnabled(this, false);

	gadu->searchNextInPubdir(*searchRecord);

	progress->setText(tr("Searching..."));
	kdebugf2();
}

void SearchDialog::newSearchResults(SearchResults& searchResults, int seq, int fromUin)
{
	kdebugf();

	QListViewItem *qlv = NULL;
	QPixmap pix;

	if ((seq != searchRecord->Seq) || searchRecord->IgnoreResults)
		return;

	searchRecord->FromUin = fromUin;

	// ??	if ((status && atoi(status) <= 1 && only_active->isChecked()) || !status)

	CONST_FOREACH(searchIterator, searchResults)
	{
		qlv = results->findItem((*searchIterator).Uin, 1);

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
			qlv = new QListViewItem(results, QString::null, (*searchIterator).Uin,
				(*searchIterator).First, (*searchIterator).City,
				(*searchIterator).Nick, (*searchIterator).Born);
//			if (count == 1 && r_uin->isChecked() && !searchhidden
//				&& (statuscode == GG_STATUS_NOT_AVAIL || statuscode == GG_STATUS_NOT_AVAIL_DESCR)) {
//				qlv->setPixmap(0, pix);
//				searchhidden = true;
//				nextSearch();
//				return;
//				}
		//	}
			qlv->setPixmap(0, pix);
			qlv = NULL;
		}
	}

	progress->setText(tr("Done searching"));

	if (!results->selectedItem())
		results->setSelected(results->firstChild(), true);

//	searchhidden = false;
	if ((r_pers->isChecked() && !isPersonalDataEmpty()) || (r_uin->isChecked() && !e_uin->text().isEmpty()))
		KaduActions["firstSearchAction"]->setEnabled(this, true);
	KaduActions["stopSearchAction"]->setEnabled(this, false);

	if (searchResults.isEmpty())
	{
		kdebugmf(KDEBUG_INFO, "No results. Exit.\n");
		MessageBox::msg(tr("There were no results of your search"), false, "NotifyTab", this);
//		searchhidden = false;
	}
	else
	{
		if (r_pers->isChecked() && !isPersonalDataEmpty())
			KaduActions["nextResultsAction"]->setEnabled(this, true);

		KaduActions["clearSearchAction"]->setEnabled(this, true);
		KaduActions["addSearchedAction"]->setEnabled(this, true);
		KaduActions["chatAction"]->setEnabled(this, true);
	}
	kdebugf2();
}

void SearchDialog::closeEvent(QCloseEvent * e)
{
	QWidget::closeEvent(e);
}

void SearchDialog::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
	results->triggerUpdate();//workaround for bug in Qt, which do not refresh results properly
}

void SearchDialog::uinTyped(void)
{
	r_uin->setChecked(true);

	KaduActions["firstSearchAction"]->setEnabled(this, !e_uin->text().isEmpty());
}

void SearchDialog::personalDataTyped(void)
{
	r_pers->setChecked(true);

	KaduActions["firstSearchAction"]->setEnabled(this, !isPersonalDataEmpty());
	KaduActions["nextResultsAction"]->setEnabled(this, false);
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
	only_active->setEnabled(true);
	only_active->setChecked(false);
	e_nick->setFocus();

	KaduActions["firstSearchAction"]->setEnabled(this, !isPersonalDataEmpty());
}

void SearchDialog::uinClicked()
{
	only_active->setEnabled(false);
	e_uin->setFocus();

	KaduActions["firstSearchAction"]->setEnabled(this, !e_uin->text().isEmpty());
	KaduActions["nextResultsAction"]->setEnabled(this, false);
}

void SearchDialog::updateInfoClicked()
{
	kdebugf();
	QListViewItem *selected = results->selectedItem();
	if (!selected && results->childCount() == 1)
		selected = results->firstChild();
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
	(new UserInfo(ule, 0, "user info"))->show();
	kdebugf2();
}

void SearchDialog::actionsAddedToToolbar(ToolButton* button, ToolBar* /*toolbar*/)
{
	kdebugf();
	if (!results->selectedItem())
		button->setEnabled(false);
	kdebugf2();
}

void SearchDialog::stopSearchActionAddedToToolbar(ToolButton* button, ToolBar* /*toolbar*/)
{
	kdebugf();
	// FIXME - przycisk powinien byc aktywny, jesli jestesmy w trakcie szukania...
	button->setEnabled(false);
	kdebugf2();
}

void SearchDialog::firstSearchActionAddedToToolbar(ToolButton* button, ToolBar* /*toolbar*/)
{
	kdebugf();
	if ((r_pers->isChecked() && isPersonalDataEmpty()) || (r_uin->isChecked() && e_uin->text().isEmpty()))
		button->setEnabled(false);
	kdebugf2();
}

void SearchDialog::nextResultsActionAddedToToolbar(ToolButton* button, ToolBar* /*toolbar*/)
{
	kdebugf();
	if (r_uin->isChecked() || isPersonalDataEmpty() || !results->selectedItem())
		button->setEnabled(false);
	kdebugf2();
}

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
	bool enableActions = results->selectedItem() != 0;
	KaduActions["addSearchedAction"]->setEnabled(this, enableActions);
	KaduActions["chatAction"]->setEnabled(this, enableActions);
}
