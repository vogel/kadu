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
#include <klocale.h>
#include <qhbuttongroup.h>
#include <stdlib.h>

//
#include "kadu.h"
#include "pixmaps.h"
#include "search.h"
#include "message.h"
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
	QLabel *l_phone;

	b_sendbtn = new QPushButton(this);
	b_sendbtn->setText(i18n("&Search"));
	b_sendbtn->setAccel(Key_Return);	
	connect(b_sendbtn, SIGNAL(clicked()), this, SLOT(doSearchWithoutStart()));

	b_nextbtn = new QPushButton(this);
	b_nextbtn->setText(i18n("&Next results"));
	connect(b_nextbtn, SIGNAL(clicked()), this, SLOT(doSearch()));

	QPushButton *b_clrbtn;
	b_clrbtn = new QPushButton(this);
	b_clrbtn->setText(i18n("C&lear list"));
	connect(b_clrbtn, SIGNAL(clicked()), this, SLOT(clearResults()));

	b_addbtn = new QPushButton(this);
	b_addbtn->setText(i18n("&Add User"));
	connect(b_addbtn, SIGNAL(clicked()), this, SLOT(AddButtonClicked()));

	QHBoxLayout* CommandLayout = new QHBoxLayout(5);
	CommandLayout->addWidget(b_sendbtn);
	CommandLayout->addWidget(b_nextbtn);
	CommandLayout->addWidget(b_clrbtn);
	CommandLayout->addWidget(b_addbtn);

	l_nick = new QLabel(this);
	l_nick->setText(i18n("Nickname"));
	e_nick = new QLineEdit(this);
	connect(e_nick, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_name = new QLabel(this);
	l_name->setText(i18n("Name"));
	e_name = new QLineEdit(this);
	connect(e_name, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_surname = new QLabel(this);
	l_surname->setText(i18n("Surname"));
	e_surname = new QLineEdit(this);
	connect(e_surname, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_gender = new QLabel(this);
	l_gender->setText(i18n("Gender"));
	c_gender = new QComboBox(this);
	c_gender->insertItem(i18n(" "), 0);
	c_gender->insertItem(i18n("Male"), 1);
	c_gender->insertItem(i18n("Female"), 2);
	connect(c_gender, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_byr = new QLabel(this);
	l_byr->setText(i18n("Birthyear"));
	e_byr = new QLineEdit(this);
	connect(e_byr, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	l_city = new QLabel(this);
	l_city->setText(i18n("City"));
	e_city = new QLineEdit(this);
	connect(e_city, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));

	QGroupBox * qgrp1 = new QGroupBox(2, Qt::Horizontal, i18n("Uin"), this);
	l_uin = new QLabel(qgrp1);
	l_uin->setText(i18n("Uin"));
	e_uin = new QLineEdit(qgrp1);
	connect(e_uin, SIGNAL(textChanged(const QString &)), this, SLOT(uinTyped()));

	QGroupBox *qgrp2 = new QGroupBox(4, Qt::Horizontal, i18n("Phone number"), this);
	l_phone = new QLabel(qgrp2);
	l_phone->setText(i18n("Phone"));
	e_phone = new QLineEdit(qgrp2);
	connect(e_phone, SIGNAL(textChanged(const QString &)), this, SLOT(phoneTyped()));

	progress = new QLabel(this);

	results = new QListView(this);
	connect(results, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(prepareMessage(QListViewItem *)));

	QHButtonGroup * btngrp = new QHButtonGroup(this);
	btngrp->setCaption(i18n("Search criteria"));
	r_pers = new QRadioButton(btngrp);
	r_pers->setText(i18n("&Personal data"));
	r_pers->setChecked(true);
	QToolTip::add(r_pers, i18n("Search using the personal data typed above (name, nickname)..."));

	r_uin = new QRadioButton(btngrp);
	r_uin->setText(i18n("&Uin number"));
	QToolTip::add(r_uin, i18n("Search for this UIN exclusively"));

	r_phone = new QRadioButton(btngrp);
	r_phone->setText(i18n("P&hone number"));
	QToolTip::add(r_phone, i18n("Search by phone"));

	connect(results, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(selectionChanged(QListViewItem *)));

	btngrp->insert(r_pers, 1);
	btngrp->insert(r_uin, 2);
	btngrp->insert(r_phone, 3);

	only_active = new QCheckBox(this);
	only_active->setText(i18n("Only active users"));

	QGridLayout * grid = new QGridLayout (this, 7, 8, 3, 3);
	grid->addMultiCellWidget(only_active, 0, 0, 0, 2);
	grid->addWidget(l_nick, 1, 0, Qt::AlignRight); grid->addWidget(e_nick, 1, 1);
	grid->addWidget(l_name, 1, 3, Qt::AlignRight); grid->addWidget(e_name, 1, 4);
	grid->addWidget(l_surname, 1, 6, Qt::AlignRight); grid->addWidget(e_surname, 1, 7);
	grid->addWidget(l_gender, 2, 0, Qt::AlignRight); grid->addWidget(c_gender, 2, 1);
	grid->addWidget(l_byr, 2, 3, Qt::AlignRight); grid->addWidget(e_byr, 2, 4);
	grid->addWidget(l_city, 2, 6, Qt::AlignRight); grid->addWidget(e_city, 2, 7);

	grid->addMultiCellWidget(qgrp1, 3, 3, 0, 3);
	grid->addMultiCellWidget(qgrp2, 3, 3, 4, 7);

	grid->addMultiCellWidget(btngrp, 4, 4, 0, 7);

	grid->addMultiCellWidget(results, 5, 5, 0, 7);
	grid->addMultiCell(CommandLayout,6,6,2,7);
	grid->addMultiCellWidget(progress, 6, 6, 0, 1);

	grid->addColSpacing(2, 10);
	grid->addColSpacing(5, 10);
	grid->addColSpacing(0, 10);

	results->addColumn(i18n(" "));
	results->addColumn(i18n("Uin"));
	results->addColumn(i18n("Name"));
	results->addColumn(i18n("Surname"));
	results->addColumn(i18n("City"));
	results->addColumn(i18n("Nickname"));
	results->addColumn(i18n("Birth year"));
	results->setAllColumnsShowFocus(true);

	/* dirty workaround for multiple showEvents */
	snr = snw = NULL;
	foo = NULL;
	r.start = 0;
	if (_whoisSearchUin) {
		r_uin->setChecked(true);
		e_uin->setText(QString::number(_whoisSearchUin));
		}
}

SearchDialog::~SearchDialog() {
	fprintf(stderr, "KK SearchDialog::~SearchDialog()\n");
}

void SearchDialog::selectionChanged(QListViewItem *item) {
	uin_t uin;

	fprintf(stderr, "KK SearchDialog::selectionChanged()\n");

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

void SearchDialog::deleteSocketNotifiers() {
	if (snr) {
		snr->setEnabled(false);
//		snr->deleteLater();
		delete snr;
		snr = NULL;
		}
	if (snw) {
		snw->setEnabled(false);
//		snw->deleteLater();
		delete snw;
		snw = NULL;
		}
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

int SearchDialog::doSearchWithoutStart(void) {
	r.start = 0;
	return doSearch();
}

int SearchDialog::doSearch(void) {
	int i;

	b_sendbtn->setEnabled(false);
	b_nextbtn->setEnabled(false);

	if (r_pers->isChecked()) {
		r.first_name = e_name->text().length() ? strdup(e_name->text().local8Bit()) : NULL;
		if (r.first_name)
			iso_to_cp((unsigned char *)r.first_name);
		r.last_name = e_surname->text().length() ? strdup(e_surname->text().local8Bit()) : NULL;
		if (r.last_name)
			iso_to_cp((unsigned char *)r.last_name);
		r.nickname = e_nick->text().length() ? strdup(e_nick->text().local8Bit()) : NULL;
		if (r.nickname)
			iso_to_cp((unsigned char *)r.nickname);
		r.city = e_city->text().length() ? strdup(e_city->text().local8Bit()) : NULL;
		if (r.city)
			iso_to_cp((unsigned char *)r.city);
		r.min_birth = e_byr->text().length() ? atoi(e_byr->text().local8Bit()) : 0;
		r.max_birth = e_byr->text().length() ? atoi(e_byr->text().local8Bit()) : 0;
		if (!strcmp(c_gender->currentText().latin1(), " "))
			r.gender = GG_GENDER_NONE;
		else
			if (!strcmp(c_gender->currentText().latin1(), "Male"))
				r.gender = GG_GENDER_MALE;
			else
				if (!strcmp(c_gender->currentText().latin1(), "Female"))
					r.gender = GG_GENDER_FEMALE;
		r.phone = NULL;
		r.uin = 0;
		r.email = NULL;
		}
	else
		if (r_uin->isChecked()) {
			r.uin = atoi(e_uin->text().latin1());
			r.first_name = NULL;
			r.last_name = NULL;
			r.nickname = NULL;
			r.city = NULL;
			r.min_birth = 0;
			r.max_birth = 0;
			r.email = NULL;
			r.phone = NULL;
			r.gender = 0;	
			}
		else
			if (r_phone->isChecked()) {
				r.uin = 0;
				r.first_name = NULL;
				r.last_name = NULL;
				r.nickname = NULL;
				r.city = NULL;
				r.min_birth = 0;
				r.max_birth = 0;
				r.email = NULL;
				r.phone = (char *) e_phone->text().latin1();
				r.gender = 0;	
				} // if r_pers

	if (only_active->isChecked())
		r.active = 1;
	else
		r.active = 0;

	progress->setText(i18n("Searching..."));
	fprintf(stderr, "KK SearchDialog::doSearch(): let the search begin\n");

	if (!(foo = gg_search(&r, 1))) {
		fprintf(stderr, "KK SearchDialog::doSearch(): gg_search() failed\n");
		QMessageBox::critical(this, i18n("Search failed"), i18n("The search failed due to an internal error") );
		progress->setText(i18n("Search failed"));
		b_sendbtn->setEnabled(true);
		b_nextbtn->setEnabled(true);
		return 1;
		}

/*	int ret;
	while ((ret = gg_search_watch_fd(foo)) >=0 && foo->state != GG_STATE_CONNECTING);

	if (ret < 0) {
		QMessageBox::critical(this, i18n("Search error"), i18n("The application encountered a network error\nand was unable to complete the search") );
		progress->setText(i18n("Error searching"));
		gg_free_search(foo);
		foo = NULL;
		b_sendbtn->setEnabled(true);
		b_nextbtn->setEnabled(true);
		return 0;
		}*/

	snr = new QSocketNotifier(foo->fd, QSocketNotifier::Read, this);
	connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	snw = new QSocketNotifier(foo->fd, QSocketNotifier::Write, this);
	connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));

	return 0;
}

void SearchDialog::dataReceived() {
	fprintf(stderr, "KK SearchDialog::dataReceived()\n");
	if (foo->check & GG_CHECK_READ)
		socketEvent();
}

void SearchDialog::dataSent() {
	fprintf(stderr, "KK SearchDialog::dataSent()\n");
	snw->setEnabled(false);
	if (foo->check & GG_CHECK_WRITE)
		socketEvent();    
}

void SearchDialog::socketEvent(void) {
	int i;

	fprintf(stderr, "KK SearchDialog::socketEvent(): Data on socket\n");

	if (gg_search_watch_fd(foo) < 0) {
		fprintf(stderr,"KK SearchDialog::socketEvent(): gg_search_watch_fd() error. bailing out.\n");
		QMessageBox::critical(this, i18n("Search error"), i18n("The application encountered a network error\nand was unable to complete the search") );
		progress->setText(i18n("Error searching"));
		deleteSocketNotifiers();
		gg_free_search(foo);
		foo = NULL;
		b_sendbtn->setEnabled(true);
		b_nextbtn->setEnabled(true);
		return;
		}

	if (foo->state == GG_STATE_CONNECTING) {
		fprintf(stderr, "KK SearchDialog::socketEvent(): changing QSocketNotifiers.\n");

		deleteSocketNotifiers();

		snr = new QSocketNotifier(foo->fd, QSocketNotifier::Read, this);
		connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

		snw = new QSocketNotifier(foo->fd, QSocketNotifier::Write, this);
		connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
		}

	res = (struct gg_search *) foo->data;

	fprintf(stderr,"KK SearchDialog::socketEvent(): res = %p\n", foo->data);
	if (foo->state == GG_STATE_DONE && res)
		fprintf(stderr,"KK SearchDialog::socketEvent(): res->results = %p\n", res->results);

	if (foo->state == GG_STATE_DONE && (res == NULL || (res && res->results == NULL))) {
		fprintf(stderr, "KK SearchDialog::socketEvent(): No results. Exit.\n");
		QMessageBox::information(this, i18n("No results"), i18n("There were no results of your search") );
		deleteSocketNotifiers();
		gg_free_search(foo);
		foo = NULL;
		b_sendbtn->setEnabled(true);
		b_nextbtn->setEnabled(true);
		return;
		}

	if (foo->state == GG_STATE_ERROR) {
		QMessageBox::critical(this, i18n("Search error"), i18n("The application encountered a network error\nand was unable to complete the search") );
		progress->setText(i18n("Error searching"));
		deleteSocketNotifiers();
		gg_free_search(foo);
		foo = NULL;
		b_sendbtn->setEnabled(true);
		b_nextbtn->setEnabled(true);
		return;
		}

	if (foo->state == GG_STATE_DONE) {
		fprintf(stderr, "KK SearchDialog::socketEvent(): Done searching\n");
		progress->setText(i18n("Done searching"));
		QString born;
		QListViewItem * qlv;
		QPixmap * qpx;
		qlv = NULL;

		for (i = 0; i < res->count; i++) {
			if (res->results[i].active)
				qpx = new QPixmap((const char **)gg_act_xpm);
			else
				if (!res->results[i].active && only_active->isChecked())
					qpx = new QPixmap((const char **)gg_busy_xpm);
				else
					qpx = new QPixmap((const char **)gg_inact_xpm);

			if (res->results[i].born)
				born = QString::number(res->results[i].born);
			else
				born = "?";

			cp_to_iso((unsigned char *)res->results[i].first_name);
			cp_to_iso((unsigned char *)res->results[i].last_name);
			cp_to_iso((unsigned char *)res->results[i].nickname);
			cp_to_iso((unsigned char *)res->results[i].city);
			qlv = new QListViewItem(results, QString::null, QString::number(res->results[i].uin),
				__c2q(res->results[i].first_name), __c2q(res->results[i].last_name),
				__c2q(res->results[i].city), __c2q(res->results[i].nickname), born);
			qlv->setPixmap(0, *qpx);
			}
		r.start = res->results[res->count-1].uin;
		if (!results->selectedItem())
			results->setSelected(results->firstChild(), true);
		else
			selectionChanged(results->selectedItem());

		deleteSocketNotifiers();
		gg_free_search(foo);
		foo = NULL;
		b_sendbtn->setEnabled(true);
		b_nextbtn->setEnabled(true);
		}
	else
		if (foo->check & GG_CHECK_WRITE)
			snw->setEnabled(true);
}

void SearchDialog::closeEvent(QCloseEvent * e) {
	deleteSocketNotifiers();
	if (foo) {
		gg_free_search(foo);
		foo = NULL;
		}	
	QWidget::closeEvent(e);
}

void SearchDialog::phoneTyped(void) {
	r_phone->setChecked(true);
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
	QString lastname = selected->text(3);
	QString nickname = selected->text(5);

	// Build altnick. Try user nick first.
	QString altnick = nickname;
	// If nick is empty, try firstname+lastname.
	if (!altnick.length()) {
		altnick = firstname;
		if (firstname.length() && lastname.length())
			altnick += " ";
		altnick += lastname;
		}
	// If nick is empty, use uin.
	if (!altnick.length())
		altnick = uin;

	if (QMessageBox::information(this, i18n("Add User"),
		i18n("Do you want to add user %1 to user list?").arg(altnick),
		i18n("&Yes"), i18n("&No")) != 0)
		return;

	kadu->addUser(firstname,lastname,nickname,altnick,"",uin,
		GG_STATUS_NOT_AVAIL,"","");
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
	QString lastname = selected->text(3);
	QString nickname = selected->text(5);

	uin_t uin = suin.toUInt();
	UserListElement &ule = userlist.byUin(uin);

	// Build altnick. Try user nick first.
	QString altnick = nickname;
	// If nick is empty, try firstname+lastname.
	if (!altnick.length()) {
		altnick = firstname;
		if (firstname.length() && lastname.length())
			altnick += " ";
		altnick += lastname;
		}
	// If nick is empty, use uin.
	if (!altnick.length())
		altnick = uin;

	if (QMessageBox::information(this, i18n("Update Info"),
		i18n("Do you want to update user info for %1?").arg(altnick),
		i18n("&Yes"), i18n("&No")) != 0)
		return;

	userlist.changeUserInfo(ule.altnick, firstname, lastname, nickname, ule.altnick,
		ule.mobile, ule.blocking, ule.offline_to_user, ule.notify, ule.group);
	userlist.writeToFile();
}

#include "search.moc"
