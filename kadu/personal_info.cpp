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
#include <qlayout.h>

// kadu.h dolaczony tylko ze wzgl. na zmienna config
// w przyszlosci trzeba bedzie chyba to zmienic
#include "kadu.h"
#include "personal_info.h"
//

PersonalInfoDialog::PersonalInfoDialog(QDialog *parent, const char *name)
	: QDialog (parent, name)
{
	fprintf(stderr,"PersonalInfoDialog::PersonalInfoDialog()\n");

	resize(650, 150);
	setCaption(i18n("Personal Information"));	
	setWFlags(Qt::WDestructiveClose);
	
	QGridLayout *GridLayout = new QGridLayout(this, 5, 6, 10, 10);

	QLabel *NicknameLabel = new QLabel(i18n("Nickname"), this);
	GridLayout->addWidget(NicknameLabel, 0, 0);
	NicknameEdit = new QLineEdit(this);
	GridLayout->addWidget(NicknameEdit, 0, 1);

	QLabel *NameLabel = new QLabel(i18n("Name"), this);
	GridLayout->addWidget(NameLabel, 0, 2);
	NameEdit = new QLineEdit(this);
	GridLayout->addWidget(NameEdit, 0, 3);
	
	QLabel *SurnameLabel = new QLabel(i18n("Surname"), this);
	GridLayout->addWidget(SurnameLabel, 0, 4);
	SurnameEdit = new QLineEdit(this);
	GridLayout->addWidget(SurnameEdit, 0, 5);

	QLabel *GenderLabel = new QLabel(i18n("Gender"),this);
	GridLayout->addWidget(GenderLabel, 1, 0);
	GenderCombo = new QComboBox(this);
	GenderCombo->insertItem("");	
	GenderCombo->insertItem(i18n("Male"));
	GenderCombo->insertItem(i18n("Female"));
	GridLayout->addWidget(GenderCombo, 1, 1);

	QLabel *BirthyearLabel = new QLabel(i18n("Birthyear"), this);
	GridLayout->addWidget(BirthyearLabel, 1, 2);
	BirthyearEdit = new QLineEdit(this);
	GridLayout->addWidget(BirthyearEdit, 1, 3);

	QLabel *CityLabel = new QLabel(i18n("City"), this);
	GridLayout->addWidget(CityLabel, 1, 4);
	CityEdit = new QLineEdit(this);
	GridLayout->addWidget(CityEdit, 1, 5);

	QLabel *FamilyNameLabel = new QLabel(i18n("Family Name"), this);
	GridLayout->addWidget(FamilyNameLabel, 2, 0);
	FamilyNameEdit = new QLineEdit(this);
	GridLayout->addWidget(FamilyNameEdit, 2, 1);

	QLabel *FamilyCityLabel = new QLabel(i18n("Family City"), this);
	GridLayout->addWidget(FamilyCityLabel, 2, 2);
	FamilyCityEdit = new QLineEdit(this);
	GridLayout->addWidget(FamilyCityEdit, 2, 3);

	QPushButton* OkButton = new QPushButton(i18n("&OK"), this);
	GridLayout->addMultiCellWidget(OkButton, 4, 4, 1, 2);
	connect(OkButton, SIGNAL(clicked()), this, SLOT(OkButtonClicked()));

	QPushButton* CancelButton = new QPushButton(i18n("&Cancel"), this);
	GridLayout->addMultiCellWidget(CancelButton, 4, 4, 3, 4);	
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	if (getActualStatus() != GG_STATUS_NOT_AVAIL) {
		struct SearchIdStruct sid;
		gg_pubdir50_t req;
		req = gg_pubdir50_new(GG_PUBDIR50_READ);
		sid.ptr = this;
		sid.seq = gg_pubdir50(sess, req);
		sid.type = DIALOG_PERSONAL;
		SearchList.append(sid);
		gg_pubdir50_free(req);
		State = READING;
		}
};

void PersonalInfoDialog::OkButtonClicked()
{
	if (getActualStatus() == GG_STATUS_NOT_AVAIL)
		return;

	char *nick = strdup(NicknameEdit->text().local8Bit());
	char *first = strdup(NameEdit->text().local8Bit());
	char *last = strdup(SurnameEdit->text().local8Bit());
	char *city = strdup(CityEdit->text().local8Bit());
	char *born = strdup(BirthyearEdit->text().local8Bit());
	char *family_name = strdup(FamilyNameEdit->text().local8Bit());
	char *family_city = strdup(FamilyCityEdit->text().local8Bit());

	iso_to_cp((unsigned char *)nick);
	iso_to_cp((unsigned char *)first);
	iso_to_cp((unsigned char *)last);
	iso_to_cp((unsigned char *)city);
	iso_to_cp((unsigned char *)family_name);
	iso_to_cp((unsigned char *)family_city);

	struct SearchIdStruct sid;
	gg_pubdir50_t req;
	req = gg_pubdir50_new(GG_PUBDIR50_WRITE);
	if (strlen(first))
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)first);
	if (strlen(last))
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)last);
	if (strlen(nick))
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)nick);
	if (strlen(city))
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)city);
	if (GenderCombo->currentItem())
		gg_pubdir50_add(req, GG_PUBDIR50_GENDER, QString::number(GenderCombo->currentItem()).latin1());
	if (strlen(born))
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)born);
	if (strlen(family_name))
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYNAME, (const char *)family_name);
	if (strlen(family_city))
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYCITY, (const char *)family_city);

	sid.ptr = this;
	sid.seq = gg_pubdir50(sess, req);
	sid.type = DIALOG_PERSONAL;
	SearchList.append(sid);
	gg_pubdir50_free(req);

	State = WRITTING;

	setEnabled(false);

	free(nick);
	free(first);
	free(last);
	free(city);
	free(born);
	free(family_name);
	free(family_city);
};

void PersonalInfoDialog::fillFields(gg_pubdir50_t res)
{
	int count;
	const char *first, *last, *nick, *born, *city,
		*gender, *family_name, *family_city;

	deleteSearchIdStruct(this);

	switch (State) {
		case READING:
			fprintf(stderr, "KK PersonalInfoDialog::fillFields(): Done reading info,\n");

			first = gg_pubdir50_get(res, 0, GG_PUBDIR50_FIRSTNAME);
			last = gg_pubdir50_get(res, 0, GG_PUBDIR50_LASTNAME);
			nick = gg_pubdir50_get(res, 0, GG_PUBDIR50_NICKNAME);
			gender = gg_pubdir50_get(res, 0, GG_PUBDIR50_GENDER);
			born = gg_pubdir50_get(res, 0, GG_PUBDIR50_BIRTHYEAR);
			city = gg_pubdir50_get(res, 0, GG_PUBDIR50_CITY);
			family_name = gg_pubdir50_get(res, 0, GG_PUBDIR50_FAMILYNAME);
			family_city = gg_pubdir50_get(res, 0, GG_PUBDIR50_FAMILYCITY);
			if (first)
				cp_to_iso((unsigned char *)first);
			if (last)
				cp_to_iso((unsigned char *)last);
			if (nick)
				cp_to_iso((unsigned char *)nick);
			if (city)
				cp_to_iso((unsigned char *)city);
			if (family_name)
				cp_to_iso((unsigned char *)family_name);
			if (family_city)
				cp_to_iso((unsigned char *)family_city);
			NicknameEdit->setText(__c2q(nick));
			NameEdit->setText(__c2q(first));
			SurnameEdit->setText(__c2q(last));
			GenderCombo->setCurrentItem(gender ? atoi(gender) : 0);
			BirthyearEdit->setText(__c2q(born));
			CityEdit->setText(__c2q(city));
			FamilyNameEdit->setText(__c2q(family_name));
			FamilyCityEdit->setText(__c2q(family_city));
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
