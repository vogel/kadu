/***************************************************************************
                          search.h  -  description
                             -------------------
    begin                : czw cze 20 19:54:52 CEST 2002
    copyright            : (C) 2002 by chilek
    email                : chilek@chilan.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SEARCH_H
#define SEARCH_H

#include <qradiobutton.h>
#include <qdialog.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qsocketnotifier.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qlabel.h>
#include "../libgadu/lib/libgadu.h"

class SearchDialog : public QDialog {
	Q_OBJECT
	public:
		SearchDialog(QDialog* parent=0, const char *name=0, uin_t whoisSearchUin = 0);
		~SearchDialog(void);

	private:
		struct gg_http *foo;
		struct gg_search *res;
		struct gg_search_request r;

		QCheckBox *only_active;
		QLineEdit *e_name;
		QLineEdit *e_nick;
		QLineEdit *e_byr;
		QLineEdit *e_surname;
		QComboBox *c_gender;
		QLineEdit *e_city;
		QListView *results;
		QLabel *progress;
		QRadioButton *r_uin;
		QRadioButton *r_pers;
		QRadioButton *r_phone;
		QRadioButton *r_mail;
		QPushButton *b_sendbtn;
		QPushButton *b_nextbtn;

		uin_t _whoisSearchUin;

		QLineEdit *e_phone;
		QLineEdit *e_mail;
		QLineEdit *e_uin;

		QSocketNotifier *snr;
		QSocketNotifier *snw;

		struct gg_event *e;

		void deleteSocketNotifiers(void);

	private slots:
		void socketEvent(void);
		void clearResults(void);
		void prepareMessage(QListViewItem*);
		void uinTyped(void);
		void phoneTyped(void);
		void dataReceived(void);
		void dataSent(void);
		void AddButtonClicked();

	public slots:
		int doSearch(void);
		int doSearchWithoutStart(void);

	protected:
		void closeEvent(QCloseEvent * e);

};
		
#endif
