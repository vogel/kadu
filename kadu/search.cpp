/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qgroupbox.h>
#include <qmessagebox.h>
#include <qhbuttongroup.h>
#include <qlayout.h>
#include <qtooltip.h>

#include <stdlib.h>
//
#include "gadu.h"
#include "status.h"
#include "search.h"
#include "events.h"
#include "userinfo.h"
#include "chat.h"
#include "debug.h"
//

SearchDialog::SearchDialog(QWidget *parent, const char *name, uin_t whoisSearchUin)
: QDialog (parent, name, FALSE, Qt::WDestructiveClose) {

	_whoisSearchUin = whoisSearchUin;

	QLabel *l_name;
	QLabel *l_nick;
	QLabel *l_byr;
	QLabel *l_surname;
	QLabel *l_gender;
	QLabel *l_city;
	QLabel *l_uin;
	
	b_chat = new QPushButton(tr("&Chat"),this);
	connect(b_chat, SIGNAL(clicked()), this, SLOT(openChat()));

	b_sendbtn = new QPushButton(tr("&Search"),this);
	b_sendbtn->setAccel(Key_Return);	
	connect(b_sendbtn, SIGNAL(clicked()), this, SLOT(firstSearch()));

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

	l_nick = new QLabel(tr("Nickname"),this);
	e_nick = new QLineEdit(this);
	connect(e_nick, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_name = new QLabel(tr("Name"),this);
	e_name = new QLineEdit(this);
	connect(e_name, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_surname = new QLabel(tr("Surname"),this);
	e_surname = new QLineEdit(this);
	connect(e_surname, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_gender = new QLabel(tr("Gender"),this);
	c_gender = new QComboBox(this);
	c_gender->insertItem(tr(" "), 0);
	c_gender->insertItem(tr("Male"), 1);
	c_gender->insertItem(tr("Female"), 2);
	connect(c_gender, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_byr = new QLabel(tr("Birthyear"),this);
	e_byr = new QLineEdit(this);
	connect(e_byr, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

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

	only_active = new QCheckBox(tr("Only active users"),this);

	QGridLayout * grid = new QGridLayout (this, 7, 8, 3, 3);
	grid->addMultiCellWidget(only_active, 0, 0, 0, 2);
	grid->addWidget(l_nick, 1, 0, Qt::AlignRight); grid->addWidget(e_nick, 1, 1);
	grid->addWidget(l_name, 1, 3, Qt::AlignRight); grid->addWidget(e_name, 1, 4);
	grid->addWidget(l_surname, 1, 6, Qt::AlignRight); grid->addWidget(e_surname, 1, 7);
	grid->addWidget(l_gender, 2, 0, Qt::AlignRight); grid->addWidget(c_gender, 2, 1);
	grid->addWidget(l_byr, 2, 3, Qt::AlignRight); grid->addWidget(e_byr, 2, 4);
	grid->addWidget(l_city, 2, 6, Qt::AlignRight); grid->addWidget(e_city, 2, 7);

	grid->addMultiCellWidget(qgrp1, 3, 3, 0, 3);

	grid->addMultiCellWidget(btngrp, 3, 3, 4, 7);

	grid->addMultiCellWidget(results, 5, 5, 0, 7);
	grid->addMultiCell(CommandLayout,6,6,2,7);
	grid->addMultiCellWidget(progress, 6, 6, 0, 1);

	grid->addColSpacing(2, 10);
	grid->addColSpacing(5, 10);
	grid->addColSpacing(0, 10);

	results->addColumn(tr("Status"));
	results->addColumn(tr("Uin"));
	results->addColumn(tr("Name"));
	results->addColumn(tr("City"));
	results->addColumn(tr("Nickname"));
	results->addColumn(tr("Birth year"));
	results->setAllColumnsShowFocus(true);
	results->setResizeMode(QListView::AllColumns);
	for (int i = 1; i < 5; i++)
		results->setColumnWidthMode(i, QListView::Maximum);

//	searchhidden = false;
	fromUin = 0;
	if (_whoisSearchUin) {
		r_uin->setChecked(true);
		e_uin->setText(QString::number(_whoisSearchUin));
		}
	resize(450,330);
	setCaption(tr("Search in directory"));
}

SearchDialog::~SearchDialog() {
	kdebug("SearchDialog::~SearchDialog()\n");
}

void SearchDialog::selectionChanged(QListViewItem *item) {
	uin_t uin;

	kdebug("SearchDialog::selectionChanged()\n");

	disconnect(b_addbtn, SIGNAL(clicked()), 0, 0);
	if (item) {
		uin = item->text(1).toUInt();
		if ((userlist.containsUin(uin) && !userlist.byUin(uin).anonymous)
			|| (userlist.containsUin(uin) && !config_file.readBoolEntry("General", "UseDocking"))) {
			b_addbtn->setText(tr("&Update Info"));
			connect(b_addbtn, SIGNAL(clicked()), this, SLOT(updateInfoClicked()));
			}
		else {
			b_addbtn->setText(tr("&Add User"));
			connect(b_addbtn, SIGNAL(clicked()), this, SLOT(AddButtonClicked()));
			}
		}
	else {
		b_addbtn->setText(tr("&Add User"));
		connect(b_addbtn, SIGNAL(clicked()), this, SLOT(AddButtonClicked()));
		}
}

void SearchDialog::prepareMessage(QListViewItem *item) {
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

void SearchDialog::clearResults(void) {
	results->clear();
}

void SearchDialog::firstSearch(void) {
	if (results->childCount())
		clearResults();
	fromUin = 0;
	nextSearch();
}

void SearchDialog::nextSearch(void) {
	int i;
	gg_pubdir50_t req;
	QString bufyear;

	if (getActualStatus() == GG_STATUS_NOT_AVAIL)
		return;

	b_sendbtn->setEnabled(false);
	b_nextbtn->setEnabled(false);

	req = gg_pubdir50_new(GG_PUBDIR50_SEARCH);

	if (r_pers->isChecked()) {
		if (e_name->text().length())
			gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)unicode2cp(e_name->text()).data());
		if (e_surname->text().length())
			gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)unicode2cp(e_surname->text()).data());
		if (e_nick->text().length())
			gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)unicode2cp(e_nick->text()).data());
		if (e_city->text().length())
			gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)unicode2cp(e_city->text()).data());
		if (e_byr->text().length()) {
			bufyear = e_byr->text() + " " + e_byr->text();
			gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)unicode2cp(bufyear).data());
			}
		switch (c_gender->currentItem()) {
			case 1:
				gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_MALE);
				break;
			case 2:
				gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_FEMALE);
				break;
			}
		}
	else
		if (r_uin->isChecked()) {
			if (e_uin->text().length())
				gg_pubdir50_add(req, GG_PUBDIR50_UIN, (const char *)unicode2cp(e_uin->text()).data());
//			if (searchhidden) {
//				bufyear = "qwertyuiopasdfghjklzxcvbnm";
//				gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)unicode2cp(bufyear.data()));
//				}
			}

	if (only_active->isChecked())
		gg_pubdir50_add(req, GG_PUBDIR50_ACTIVE, GG_PUBDIR50_ACTIVE_TRUE);
	QString s;
	s = QString::number(fromUin);
	gg_pubdir50_add(req, GG_PUBDIR50_START, s.local8Bit());

	progress->setText(tr("Searching..."));
	kdebug("SearchDialog::doSearch(): let the search begin\n");

	seq = gg_pubdir50(sess, req);
	gg_pubdir50_free(req);
	connect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(showResults(gg_pubdir50_t)));
}

void SearchDialog::showResults(gg_pubdir50_t res) {
	int i, j, count, statuscode;
	const char *uin, *first, *nick, *born, *city, *status;
	
	if (res->seq != seq)
		return;

	disconnect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(showResults(gg_pubdir50_t)));

	count = gg_pubdir50_count(res);

	if (count < 1) {
		kdebug("SearchDialog::showResults(): No results. Exit.\n");
		progress->setText(tr("Done searching"));
		QMessageBox::information(this, tr("No results"),
			tr("There were no results of your search"));
//		searchhidden = false;
		b_sendbtn->setEnabled(true);
		b_nextbtn->setEnabled(true);
		return;
		}

	kdebug("SearchDialog::showResults(): Done searching. count=%d\n", count);
	QListViewItem * qlv;
	QPixmap pix;
	qlv = NULL;

	for (i = 0; i < count; i++) {
		uin = gg_pubdir50_get(res, i, GG_PUBDIR50_UIN);
		first = gg_pubdir50_get(res, i, GG_PUBDIR50_FIRSTNAME);
		nick = gg_pubdir50_get(res, i, GG_PUBDIR50_NICKNAME);
		born = gg_pubdir50_get(res, i, GG_PUBDIR50_BIRTHYEAR);
		city = gg_pubdir50_get(res, i, GG_PUBDIR50_CITY);
		status = gg_pubdir50_get(res, i, GG_PUBDIR50_STATUS);
		if ((status && atoi(status) <= 1 && only_active->isChecked()) || !status)
			continue;
		qlv = results->findItem(uin, 1);
		statuscode = atoi(status) & 127;
		if (statuscode)
			pix = icons_manager.loadIcon(gg_icons[statusGGToStatusNr(statuscode)]);
		else
			pix = icons_manager.loadIcon("Offline");
		if (qlv) {
//			if (!searchhidden) {
				qlv->setText(1, cp2unicode((unsigned char *)uin));
				qlv->setText(2, cp2unicode((unsigned char *)first));
				qlv->setText(3, cp2unicode((unsigned char *)city));
				qlv->setText(4, cp2unicode((unsigned char *)nick));
				qlv->setText(5, cp2unicode((unsigned char *)born));
//				}
//			else
//				searchhidden = false;
			}
		else {
			qlv = new QListViewItem(results, QString::null, cp2unicode((unsigned char *)uin),
				cp2unicode((unsigned char *)first), cp2unicode((unsigned char *)city),
				cp2unicode((unsigned char *)nick), cp2unicode((unsigned char *)born));
//			if (count == 1 && r_uin->isChecked() && !searchhidden
//				&& (statuscode == GG_STATUS_NOT_AVAIL || statuscode == GG_STATUS_NOT_AVAIL_DESCR)) {
//				qlv->setPixmap(0, pix);
//				searchhidden = true;
//				nextSearch();
//				return;
//				}
			}
		qlv->setPixmap(0, pix);
		qlv = NULL;
		}

	progress->setText(tr("Done searching"));

	fromUin = gg_pubdir50_next(res);
	if (!results->selectedItem())
		results->setSelected(results->firstChild(), true);
	else
		selectionChanged(results->selectedItem());

//	searchhidden = false;
	b_sendbtn->setEnabled(true);
	b_nextbtn->setEnabled(true);
}

void SearchDialog::closeEvent(QCloseEvent * e) {
	disconnect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(showResults(gg_pubdir50_t)));
	QWidget::closeEvent(e);
}

void SearchDialog::uinTyped(void) {
	r_uin->setChecked(true);
}

void SearchDialog::personalDataTyped(void) {
	r_pers->setChecked(true);
}

void SearchDialog::AddButtonClicked()
{
	QListViewItem *selected = results->selectedItem();
	if (!selected && results->childCount() == 1)
		selected = results->firstChild();
	if (!selected) {
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
	if (!altnick.length()) {
		altnick = firstname;
//		if (firstname.length() && lastname.length())
//			altnick += " ";
//		altnick += lastname;
		}
	// If nick is empty, use uin.
	if (!altnick.length())
		altnick = uin;

	UserInfo *ui = new UserInfo("user info", 0, QString::null, true);
	UserListElement e;
	bool ok;
	e.first_name = firstname;
	e.last_name = "";
	e.nickname = nickname;
	e.altnick = altnick;
	e.mobile = "";
	e.uin = uin.toUInt(&ok);
	if (!ok)
		e.uin = 0;
	e.setGroup("");
	e.description = "";
	e.email = "";
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
}

void SearchDialog::updateInfoClicked()
{
	QListViewItem *selected = results->selectedItem();
	if (!selected && results->childCount() == 1)
		selected = results->firstChild();
	if (!selected)
		return;

	QString suin = selected->text(1);
	QString firstname = selected->text(2);
//	QString lastname = selected->text(3);
	QString nickname = selected->text(4);

	uin_t uin = suin.toUInt();
	UserListElement &ule = userlist.byUin(uin);

	// Build altnick. Try user nick first.
	QString altnick = nickname;
	// If nick is empty, try firstname+lastname.
	if (!altnick.length()) {
		altnick = firstname;
//		if (firstname.length() && lastname.length())
//			altnick += " ";
//		altnick += lastname;
		}
	// If nick is empty, use uin.
	if (!altnick.length())
		altnick = uin;

	UserListElement e;
	e.first_name = firstname;
	e.last_name = ule.last_name;
	e.nickname = nickname;
	e.altnick = ule.altnick;
	e.mobile = ule.mobile;
	e.uin = ule.uin;
	e.status = ule.status;
	e.image_size = ule.image_size;
	e.blocking = ule.blocking;
	e.offline_to_user = ule.offline_to_user;
	e.notify = ule.notify;
	e.setGroup(ule.group());
	e.email = ule.email;
	UserInfo *ui = new UserInfo("user info", 0, ule.altnick);
	ui->setUserInfo(e);
	ui->show();
}

void SearchDialog::openChat()
{
	QListViewItem *selected = results->selectedItem();
	if (!selected && results->childCount() == 1)
		selected = results->firstChild();
	if (!selected)
		return;

	QString uin = selected->text(1);
	UinsList uins;

	uins.append((uin_t)uin.toInt());

	chat_manager->openChat(uins);
}
