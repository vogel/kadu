/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qhbuttongroup.h>
#include <stdlib.h>

//
#include "kadu.h"
#include "pixmaps.h"
#include "search.h"
#include "message.h"
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

	b_sendbtn = new QPushButton(i18n("&Search"),this);
	b_sendbtn->setAccel(Key_Return);	
	connect(b_sendbtn, SIGNAL(clicked()), this, SLOT(firstSearch()));

	b_nextbtn = new QPushButton(i18n("&Next results"),this);
	connect(b_nextbtn, SIGNAL(clicked()), this, SLOT(nextSearch()));

	QPushButton *b_clrbtn;
	b_clrbtn = new QPushButton(i18n("C&lear list"),this);
	connect(b_clrbtn, SIGNAL(clicked()), this, SLOT(clearResults()));

	b_addbtn = new QPushButton(i18n("&Add User"),this);
	connect(b_addbtn, SIGNAL(clicked()), this, SLOT(AddButtonClicked()));

	QHBoxLayout* CommandLayout = new QHBoxLayout(5);
	CommandLayout->addWidget(b_sendbtn);
	CommandLayout->addWidget(b_nextbtn);
	CommandLayout->addWidget(b_clrbtn);
	CommandLayout->addWidget(b_addbtn);

	l_nick = new QLabel(i18n("Nickname"),this);
	e_nick = new QLineEdit(this);
	connect(e_nick, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_name = new QLabel(i18n("Name"),this);
	e_name = new QLineEdit(this);
	connect(e_name, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_surname = new QLabel(i18n("Surname"),this);
	e_surname = new QLineEdit(this);
	connect(e_surname, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_gender = new QLabel(i18n("Gender"),this);
	c_gender = new QComboBox(this);
	c_gender->insertItem(i18n(" "), 0);
	c_gender->insertItem(i18n("Male"), 1);
	c_gender->insertItem(i18n("Female"), 2);
	connect(c_gender, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_byr = new QLabel(i18n("Birthyear"),this);
	e_byr = new QLineEdit(this);
	connect(e_byr, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_city = new QLabel(i18n("City"),this);
	e_city = new QLineEdit(this);
	connect(e_city, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	QGroupBox * qgrp1 = new QGroupBox(2, Qt::Horizontal, i18n("Uin"), this);
	l_uin = new QLabel(i18n("Uin"),qgrp1);
	e_uin = new QLineEdit(qgrp1);
	connect(e_uin, SIGNAL(textChanged(const QString &)), this, SLOT(uinTyped()));

	progress = new QLabel(this);

	results = new QListView(this);
	connect(results, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(prepareMessage(QListViewItem *)));

	QHButtonGroup * btngrp = new QHButtonGroup(this);
	btngrp->setTitle(i18n("Search criteria"));
	r_pers = new QRadioButton(i18n("&Personal data"),btngrp);
	r_pers->setChecked(true);
	QToolTip::add(r_pers, i18n("Search using the personal data typed above (name, nickname)..."));

	r_uin = new QRadioButton(i18n("&Uin number"),btngrp);
	QToolTip::add(r_uin, i18n("Search for this UIN exclusively"));

	connect(results, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(selectionChanged(QListViewItem *)));

	btngrp->insert(r_pers, 1);
	btngrp->insert(r_uin, 2);

	only_active = new QCheckBox(i18n("Only active users"),this);

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

	results->addColumn(i18n("Status"));
	results->addColumn(i18n("Uin"));
	results->addColumn(i18n("Name"));
	results->addColumn(i18n("City"));
	results->addColumn(i18n("Nickname"));
	results->addColumn(i18n("Birth year"));
	results->setAllColumnsShowFocus(true);
	results->setResizeMode(QListView::AllColumns);
	for (int i = 1; i < 5; i++)
		results->setColumnWidthMode(i, QListView::Maximum);

	fromUin = 0;
	if (_whoisSearchUin) {
		r_uin->setChecked(true);
		e_uin->setText(QString::number(_whoisSearchUin));
		}
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
		if (userlist.containsUin(uin) && !userlist.byUin(uin).anonymous) {
			b_addbtn->setText(i18n("&Update Info"));
			connect(b_addbtn, SIGNAL(clicked()), this, SLOT(updateInfoClicked()));
			}
		else {
			b_addbtn->setText(i18n("&Add User"));
			connect(b_addbtn, SIGNAL(clicked()), this, SLOT(AddButtonClicked()));
			}
		}
	else {
		b_addbtn->setText(i18n("&Add User"));
		connect(b_addbtn, SIGNAL(clicked()), this, SLOT(AddButtonClicked()));
		}
}

void SearchDialog::init() {
	resize(450,330);
	setCaption(i18n("Search in directory"));
}

void SearchDialog::prepareMessage(QListViewItem *item) {
	Message *msg;

	if (!userlist.containsUin(atoi(item->text(1).local8Bit())))
		AddButtonClicked();
	else {
		msg = new Message(userlist.byUin(atoi(item->text(1).local8Bit())).altnick);
		msg->init();
		msg->show();
		}
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
			gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)native2cp(e_name->text()).data());
		if (e_surname->text().length())
			gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)native2cp(e_surname->text()).data());
		if (e_nick->text().length())
			gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)native2cp(e_nick->text()).data());
		if (e_city->text().length())
			gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)native2cp(e_city->text()).data());
		if (e_byr->text().length()) {
			bufyear = e_byr->text() + " " + e_byr->text();
			gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)native2cp(bufyear).data());
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
				gg_pubdir50_add(req, GG_PUBDIR50_UIN, (const char *)native2cp(e_uin->text()).data());
			}

	if (only_active->isChecked())
		gg_pubdir50_add(req, GG_PUBDIR50_ACTIVE, GG_PUBDIR50_ACTIVE_TRUE);
	QString s;
	s = QString::number(fromUin);
	gg_pubdir50_add(req, GG_PUBDIR50_START, s.local8Bit());

	progress->setText(i18n("Searching..."));
	kdebug("SearchDialog::doSearch(): let the search begin\n");

	struct SearchIdStruct sid;
	sid.ptr = this;
	sid.seq = gg_pubdir50(sess, req);
	sid.type = DIALOG_SEARCH;
	SearchList.append(sid);
	gg_pubdir50_free(req);
}

void SearchDialog::showResults(gg_pubdir50_t res) {
	int i, count;
	const char *uin, *first, *nick, *born, *city, *status;
	
	count = gg_pubdir50_count(res);

	if (count < 1) {
		kdebug("SearchDialog::showResults(): No results. Exit.\n");
		QMessageBox::information(this, i18n("No results"), i18n("There were no results of your search"));
		b_sendbtn->setEnabled(true);
		b_nextbtn->setEnabled(true);
		return;
		}

	kdebug("SearchDialog::showResults(): Done searching. count=%d\n", count);
	progress->setText(i18n("Done searching"));
	QListViewItem * qlv;
	QPixmap *pix;
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
		if (results->findItem(uin, 1))
			continue;
		if (atoi(status))
			pix = icons->loadIcon(gg_icons[statusGGToStatusNr(atoi(status) & 127)]);
		else
			pix = icons->loadIcon("offline");
		qlv = new QListViewItem(results, QString::null, cp2native((unsigned char *)uin),
			cp2native((unsigned char *)first), cp2native((unsigned char *)city),
			cp2native((unsigned char *)nick), cp2native((unsigned char *)born));
		qlv->setPixmap(0, *pix);
		}

	fromUin = gg_pubdir50_next(res);
	if (!results->selectedItem())
		results->setSelected(results->firstChild(), true);
	else
		selectionChanged(results->selectedItem());

	deleteSearchIdStruct(this);
	b_sendbtn->setEnabled(true);
	b_nextbtn->setEnabled(true);
}

void SearchDialog::closeEvent(QCloseEvent * e) {
	deleteSearchIdStruct(this);
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
		QMessageBox::information(this,i18n("Add User"),i18n("Select user first"));
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

	if (QMessageBox::information(this, i18n("Add User"),
		i18n("Do you want to add user %1 to user list?").arg(altnick),
		i18n("&Yes"), i18n("&No")) != 0)
		return;

	kadu->addUser(firstname, "", nickname, altnick, "", uin,
		GG_STATUS_NOT_AVAIL, "", "");
	selectionChanged(selected);
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

	if (QMessageBox::information(this, i18n("Update Info"),
		i18n("Do you want to update user info for %1?").arg(altnick),
		i18n("&Yes"), i18n("&No")) != 0)
		return;

	userlist.changeUserInfo(ule.altnick, firstname, "", nickname, ule.altnick,
		ule.mobile, QString::number(ule.uin), ule.status,
		ule.blocking, ule.offline_to_user, ule.notify, ule.group);
	userlist.writeToFile();
}

