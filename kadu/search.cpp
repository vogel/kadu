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
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtooltip.h>

#include <stdlib.h>

#include "chat.h"
#include "debug.h"
#include "gadu.h"
#include "kadu.h"
#include "search.h"
#include "status.h"
#include "userinfo.h"
#include "userlist.h"

SearchDialog::SearchDialog(QWidget *parent, const char *name, UinType whoisSearchUin)
: QDialog (parent, name, FALSE, Qt::WDestructiveClose)
{
	kdebugf();

	_whoisSearchUin = whoisSearchUin;

	QLabel *l_name;
	QLabel *l_nick;
	QLabel *l_byr;
	QLabel *l_byrFrom;
	QLabel *l_byrTo;
	QLabel *l_surname;
	QLabel *l_gender;
	QLabel *l_city;
	QLabel *l_uin;

	b_sendbtn = new QPushButton(tr("&Search"),this);
	b_sendbtn->setAccel(Key_Return);
	connect(b_sendbtn, SIGNAL(clicked()), this, SLOT(firstSearch()));

	b_chat = new QPushButton(tr("&Chat"),this);
	connect(b_chat, SIGNAL(clicked()), this, SLOT(openChat()));

	b_nextbtn = new QPushButton(tr("&Next results"),this);
	connect(b_nextbtn, SIGNAL(clicked()), this, SLOT(nextSearch()));

	QPushButton *b_clrbtn;
	b_clrbtn = new QPushButton(tr("C&lear list"),this);
	connect(b_clrbtn, SIGNAL(clicked()), this, SLOT(clearResults()));

	b_addbtn = new QPushButton(tr("&Add User"),this);
	connect(b_addbtn, SIGNAL(clicked()), this, SLOT(AddButtonClicked()));

	QHBoxLayout* CommandLayout = new QHBoxLayout(5);
	CommandLayout->addWidget(b_chat);
	CommandLayout->addWidget(b_sendbtn);
	CommandLayout->addWidget(b_nextbtn);
	CommandLayout->addWidget(b_clrbtn);
	CommandLayout->addWidget(b_addbtn);

	only_active = new QCheckBox(tr("Only active users"),this);

	l_nick = new QLabel(tr("Nickname"),this);
	e_nick = new QLineEdit(this);
	connect(e_nick, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_gender = new QLabel(tr("Gender"),this);
	c_gender = new QComboBox(this);
	c_gender->insertItem(" ", 0);
	c_gender->insertItem(tr("Male"), 1);
	c_gender->insertItem(tr("Female"), 2);
	connect(c_gender, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_name = new QLabel(tr("Name"),this);
	e_name = new QLineEdit(this);
	connect(e_name, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_surname = new QLabel(tr("Surname"),this);
	e_surname = new QLineEdit(this);
	connect(e_surname, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_byr = new QLabel(tr("Birthyear"),this);
	l_byrFrom = new QLabel(tr("from"),this);
	e_byrFrom = new QLineEdit(this);
	connect(e_byrFrom, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	l_byrTo = new QLabel(tr("to"),this);
	e_byrTo = new QLineEdit(this);
	connect(e_byrTo, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_city = new QLabel(tr("City"),this);
	e_city = new QLineEdit(this);
	connect(e_city, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	QGroupBox * qgrp1 = new QGroupBox(2, Qt::Horizontal, tr("Uin"), this);
	l_uin = new QLabel(tr("Uin"),qgrp1);
	e_uin = new QLineEdit(qgrp1);
	connect(e_uin, SIGNAL(textChanged(const QString &)), this, SLOT(uinTyped()));

	progress = new QLabel(this);

	results = new QListView(this);
	connect(results, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(prepareMessage(QListViewItem *)));

	QHButtonGroup * btngrp = new QHButtonGroup(this);
	btngrp->setTitle(tr("Search criteria"));
	r_pers = new QRadioButton(tr("&Personal data"),btngrp);
	r_pers->setChecked(true);
	QToolTip::add(r_pers, tr("Search using the personal data typed above (name, nickname)..."));

	r_uin = new QRadioButton(tr("&Uin number"),btngrp);
	QToolTip::add(r_uin, tr("Search for this UIN exclusively"));

	connect(results, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(selectionChanged(QListViewItem *)));

	btngrp->insert(r_pers, 1);
	btngrp->insert(r_uin, 2);

	QGridLayout * grid = new QGridLayout (this, 7, 12, 3, 3);
	grid->addMultiCellWidget(only_active, 0, 0, 0, 2);
	grid->addWidget(l_nick, 1, 0, Qt::AlignRight); grid->addWidget(e_nick, 1, 1);
	grid->addWidget(l_gender, 2, 0, Qt::AlignRight); grid->addWidget(c_gender, 2, 1);
	grid->addWidget(l_name, 1, 3, Qt::AlignRight); grid->addWidget(e_name, 1, 4);
	grid->addWidget(l_surname, 2, 3, Qt::AlignRight); grid->addWidget(e_surname, 2, 4);
	grid->addWidget(l_byr, 1, 6, Qt::AlignRight);
	grid->addWidget(l_byrFrom, 1, 7, Qt::AlignRight); grid->addWidget(e_byrFrom, 1, 8);
	grid->addWidget(l_byrTo, 2, 7, Qt::AlignRight); grid->addWidget(e_byrTo, 2, 8);
	grid->addWidget(l_city, 1, 10, Qt::AlignRight); grid->addWidget(e_city, 1, 11);

	grid->addMultiCellWidget(qgrp1, 3, 3, 0, 3);

	grid->addMultiCellWidget(btngrp, 3, 3, 4, 11);

	grid->addMultiCellWidget(results, 5, 5, 0, 11);
	grid->addMultiCell(CommandLayout, 6, 6, 2, 11);
	grid->addMultiCellWidget(progress, 6, 6, 0, 1);

	grid->addColSpacing(2, 10);
	grid->addColSpacing(5, 10);
	grid->addColSpacing(9, 10);

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

//	searchhidden = false;
	if (_whoisSearchUin)
	{
		r_uin->setChecked(true);
		e_uin->setText(QString::number(_whoisSearchUin));
	}
	resize(450,330);
	setCaption(tr("Search in directory"));

	searchRecord = new SearchRecord;
	connect(gadu, SIGNAL(newSearchResults(SearchResults &, int, int)), this, SLOT(newSearchResults(SearchResults &, int, int)));
	kdebugf2();
}

SearchDialog::~SearchDialog()
{
	kdebugf();

	disconnect(gadu, SIGNAL(newSearchResults(SearchResults&, int, int)), this, SLOT(newSearchResults(SearchResults&, int, int)));
	delete searchRecord;
	kdebugf2();
}

void SearchDialog::selectionChanged(QListViewItem *item)
{
	kdebugf();

	UinType uin;

	disconnect(b_addbtn, SIGNAL(clicked()), 0, 0);
	if (item)
	{
		uin = item->text(1).toUInt();
		if ((userlist.containsUin(uin) && !userlist.byUin(uin).isAnonymous())
			|| (userlist.containsUin(uin) && !kadu->docked()))
		{
			b_addbtn->setText(tr("&Update Info"));
			connect(b_addbtn, SIGNAL(clicked()), this, SLOT(updateInfoClicked()));
		}
		else
		{
			b_addbtn->setText(tr("&Add User"));
			connect(b_addbtn, SIGNAL(clicked()), this, SLOT(AddButtonClicked()));
		}
	}
	else
	{
		b_addbtn->setText(tr("&Add User"));
		connect(b_addbtn, SIGNAL(clicked()), this, SLOT(AddButtonClicked()));
	}
	kdebugf2();
}

void SearchDialog::prepareMessage(QListViewItem * /*item*/)
{
//Tu trzeba dodaæ kod który otwiera³by okno rozmowy.
/*	Message *msg;

	if (!userlist.containsUin(atoi(item->text(1).local8Bit())))
		AddButtonClicked();
	else {
		msg = new Message(userlist.byUin(atoi(item->text(1).local8Bit())).altnick);
		msg->init();
		msg->show();
		}
*/

}

void SearchDialog::clearResults(void)
{
	results->clear();
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

	}
	else
		if (r_uin->isChecked())
			searchRecord->reqUin(e_uin->text());

	if (only_active->isChecked())
		searchRecord->reqActive();

	gadu->searchInPubdir(*searchRecord);

	b_sendbtn->setEnabled(false);
	b_nextbtn->setEnabled(false);

	progress->setText(tr("Searching..."));
	kdebugf2();
}

void SearchDialog::nextSearch(void)
{
	if (gadu->status().isOffline())
		return;
	kdebugf();

	b_sendbtn->setEnabled(false);
	b_nextbtn->setEnabled(false);

	gadu->searchNextInPubdir(*searchRecord);

	progress->setText(tr("Searching..."));
	kdebugf2();
}

void SearchDialog::newSearchResults(SearchResults& searchResults, int seq, int fromUin)
{
	kdebugf();

	QListViewItem *qlv = NULL;
	QPixmap pix;

	if (seq != searchRecord->Seq)
		return;

	searchRecord->FromUin = fromUin;

	// ??	if ((status && atoi(status) <= 1 && only_active->isChecked()) || !status)

	FOREACH(searchIterator, searchResults)
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
	else
		selectionChanged(results->selectedItem());

//	searchhidden = false;
	b_sendbtn->setEnabled(true);
	b_nextbtn->setEnabled(true);

	if (!searchResults.count())
	{
		kdebugmf(KDEBUG_INFO, "No results. Exit.\n");
		QMessageBox::information(this, tr("No results"),
			tr("There were no results of your search"));
//		searchhidden = false;
	}
	kdebugf2();
}

void SearchDialog::closeEvent(QCloseEvent * e)
{
	QWidget::closeEvent(e);
}

void SearchDialog::uinTyped(void)
{
	r_uin->setChecked(true);
}

void SearchDialog::personalDataTyped(void)
{
	r_pers->setChecked(true);
}

void SearchDialog::AddButtonClicked()
{
	kdebugf();
	QListViewItem *selected = results->selectedItem();
	if (!selected && results->childCount() == 1)
		selected = results->firstChild();
	if (!selected)
	{
		QMessageBox::information(this,tr("Add User"),
			tr("Select user first"));
		return;
	}

	QString uin = selected->text(1);
	QString firstname = selected->text(2);
	QString nickname = selected->text(4);

	// Build altnick. Try user nick first.
	QString altnick = nickname;
	// If nick is empty, try firstname+lastname.
	if (!altnick.length())
	{
		altnick = firstname;
//		if (firstname.length() && lastname.length())
//			altnick += " ";
//		altnick += lastname;
	}
	// If nick is empty, use uin.
	if (!altnick.length())
		altnick = uin;

	UserInfo *ui = new UserInfo(QString::null, true, 0, "user info");
	UserListElement e;
	bool ok;
	e.setFirstName(firstname);
	e.setLastName("");
	e.setNickName(nickname);
	e.setAltNick(altnick);
	e.setUin(uin.toUInt(&ok));
	if (!ok)
		e.setUin(0);
	e.setGroup("");
	e.setEmail("");
	ui->setUserInfo(e);
	ui->show();
	/*
		FIXME !!
	//selectionChanged(selected); - to nic nie daje
	- funkcja ta powinna byc wywolana po zamknieciu okna dodawania usera
	a jest wywolywana zaraz po pokazaniu sie okienka z dodawaniem usera
	tip: dodaæ do okna z dodawaniem sygnal który by wemitowa³ sie
	gdy dodamy nowego usera i siê pod ten sygna³ podpi±æ i wywo³aæ t± funkcje

	*/
	kdebugf2();
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
	UserListElement &ule = userlist.byUin(uin);

	// Build altnick. Try user nick first.
	QString altnick = nickname;
	// If nick is empty, try firstname+lastname.
	if (!altnick.length())
	{
		altnick = firstname;
//		if (firstname.length() && lastname.length())
//			altnick += " ";
//		altnick += lastname;
	}
	// If nick is empty, use uin.
	if (!altnick.length())
		altnick = uin;

	UserListElement e;
	e = ule;
	e.setFirstName(firstname);
	e.setNickName(nickname);
	UserInfo *ui = new UserInfo(ule.altNick(), false, 0, "user info");
	ui->setUserInfo(e);
	ui->show();
	kdebugf2();
}

void SearchDialog::openChat()
{
	kdebugf();
	QListViewItem *selected = results->selectedItem();
	if (!selected && results->childCount() == 1)
		selected = results->firstChild();
	if (!selected)
	{
		kdebugf2();
		return;
	}

	QString uin = selected->text(1);
	UinsList uins;

	uins.append((UinType)uin.toInt());

	if (uins.findIndex(config_file.readNumEntry("General", "UIN")) == -1)
		chat_manager->openChat(uins);
	kdebugf2();
}
