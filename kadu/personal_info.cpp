/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <klocale.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qmessagebox.h>

// kadu.h dolaczony tylko ze wzgl. na zmienna config
// w przyszlosci trzeba bedzie chyba to zmienic
#include "kadu.h"
#include "personal_info.h"
//

PersonalInfoDialog::PersonalInfoDialog(QDialog *parent, const char *name)
	: QDialog (parent, name)
{
	fprintf(stderr,"PersonalInfoDialog::PersonalInfoDialog()\n");

	resize(500, 150);
	setCaption(i18n("Personal Information"));	
	setWFlags(Qt::WDestructiveClose);
	
	QGridLayout* GridLayout=new QGridLayout(this, 4, 6, 10, 10);

	QLabel* NicknameLabel=new QLabel(i18n("Nickname"),this);
	GridLayout->addWidget(NicknameLabel,0,0);
	
	QLabel* NameLabel=new QLabel(i18n("Name"),this);
	GridLayout->addWidget(NameLabel,0,2);
	
	QLabel* SurnameLabel=new QLabel(i18n("Surname"),this);
	GridLayout->addWidget(SurnameLabel,0,4);

	QLabel* GenderLabel=new QLabel(i18n("Gender"),this);
	GridLayout->addWidget(GenderLabel,1,0);

	QLabel* BirthyearLabel=new QLabel(i18n("Birthyear"),this);
	GridLayout->addWidget(BirthyearLabel,1,2);

	QLabel* CityLabel=new QLabel(i18n("City"),this);
	GridLayout->addWidget(CityLabel,1,4);

	NicknameEdit=new QLineEdit(this);
	GridLayout->addWidget(NicknameEdit,0,1);

	NameEdit=new QLineEdit(this);
	GridLayout->addWidget(NameEdit,0,3);

	SurnameEdit=new QLineEdit(this);
	GridLayout->addWidget(SurnameEdit,0,5);

	GenderCombo=new QComboBox(this);
	GenderCombo->insertItem("");	
	GenderCombo->insertItem(i18n("Male"));
	GenderCombo->insertItem(i18n("Female"));
	GridLayout->addWidget(GenderCombo,1,1);

	BirthyearEdit=new QLineEdit(this);
	GridLayout->addWidget(BirthyearEdit,1,3);

	CityEdit=new QLineEdit(this);
	GridLayout->addWidget(CityEdit,1,5);

	QPushButton* OkButton=new QPushButton(i18n("&OK"),this);
	GridLayout->addMultiCellWidget(OkButton,3,3,1,2);
	connect(OkButton, SIGNAL(clicked()), this, SLOT(OkButtonClicked()));

	QPushButton* CancelButton=new QPushButton(i18n("&Cancel"),this);
	GridLayout->addMultiCellWidget(CancelButton,3,3,3,4);	
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	if (sess) {
		struct SearchIdStruct sid;
		gg_search50_t req;
		req = gg_search50_new();
		sid.ptr = this;
		sid.seq = gg_search50(sess, req, 0x02);
		sid.type = DIALOG_PERSONAL;
		SearchList.append(sid);
		gg_search50_free(req);
		State = READING;
		}
};

void PersonalInfoDialog::OkButtonClicked()
{
	char *nick = strdup(NicknameEdit->text().local8Bit());
	char *first = strdup(NameEdit->text().local8Bit());
	char *last = strdup(SurnameEdit->text().local8Bit());
	char *city = strdup(CityEdit->text().local8Bit());
	char *born = strdup(BirthyearEdit->text().local8Bit());

	iso_to_cp((unsigned char *)nick);
	iso_to_cp((unsigned char *)first);
	iso_to_cp((unsigned char *)last);
	iso_to_cp((unsigned char *)city);

	struct SearchIdStruct sid;
	gg_search50_t req;
	req = gg_search50_new();
	if (strlen(first))
		gg_search50_add(req, GG_SEARCH50_FIRSTNAME, (const char *)first);
	if (strlen(last))
		gg_search50_add(req, GG_SEARCH50_LASTNAME, (const char *)last);
	if (strlen(nick))
		gg_search50_add(req, GG_SEARCH50_NICKNAME, (const char *)nick);
	if (strlen(city))
		gg_search50_add(req, GG_SEARCH50_CITY, (const char *)city);
	if (GenderCombo->currentItem())
		gg_search50_add(req, GG_SEARCH50_GENDER, QString::number(GenderCombo->currentItem()).latin1());
	if (strlen(born))
		gg_search50_add(req, GG_SEARCH50_BIRTHYEAR, (const char *)born);
	sid.ptr = this;
	sid.seq = gg_search50(sess, req, 0x01);
	sid.type = DIALOG_PERSONAL;
	SearchList.append(sid);
	gg_search50_free(req);

	State = WRITTING;

	setEnabled(false);

	delete nick;
	delete first;
	delete last;
	delete city;
	delete born;
};

void PersonalInfoDialog::fillFields(gg_search50_t res)
{
	int count;
	const char *first, *last, *nick, *born, *city, *gender;

	deleteSearchIdStruct(this);

	switch (State) {
		case READING:
			fprintf(stderr, "KK PersonalInfoDialog::fillFields(): Done reading info,\n");

			first = gg_search50_get(res, 0, GG_SEARCH50_FIRSTNAME);
			last = gg_search50_get(res, 0, GG_SEARCH50_LASTNAME);
			nick = gg_search50_get(res, 0, GG_SEARCH50_NICKNAME);
			gender = gg_search50_get(res, 0, GG_SEARCH50_GENDER);
			born = gg_search50_get(res, 0, GG_SEARCH50_BIRTHYEAR);
			city = gg_search50_get(res, 0, GG_SEARCH50_CITY);
			if (first)
				cp_to_iso((unsigned char *)first);
			if (last)
				cp_to_iso((unsigned char *)last);
			if (nick)
				cp_to_iso((unsigned char *)nick);
			if (city)
				cp_to_iso((unsigned char *)city);
			NicknameEdit->setText(__c2q(nick));
			NameEdit->setText(__c2q(first));
			SurnameEdit->setText(__c2q(last));
			GenderCombo->setCurrentItem(gender ? atoi(gender) : 0);
			BirthyearEdit->setText(__c2q(born));
			CityEdit->setText(__c2q(city));
			break;
		case WRITTING:
			fprintf(stderr, "KK PersonalInfoDialog::fillFields(): Done writing info.\n");
			accept();
			break;
		}

	setEnabled(true);
}

void PersonalInfoDialog::closeEvent(QCloseEvent * e)
{
	deleteSearchIdStruct(this);
	QWidget::closeEvent(e);
}

#include "personal_info.moc"
