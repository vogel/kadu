/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlabel.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qvbox.h>
#include <qlayout.h>

#include <stdlib.h>

#include "gadu.h"
#include "status.h"
#include "debug.h"
#include "events.h"
#include "personal_info.h"

PersonalInfoDialog::PersonalInfoDialog(QDialog *parent, const char *name)
	: QDialog (parent, name)
{
	kdebug("PersonalInfoDialog::PersonalInfoDialog()\n");

	resize(400, 150);
	setCaption(tr("Personal Information"));	
	setWFlags(Qt::WDestructiveClose);
	
	QGridLayout *GridLayout = new QGridLayout(this, 8, 3, 10, 10);

	QVBox *NicknameBox = new QVBox(this);
	QLabel *NicknameLabel = new QLabel(tr("Nickname"), NicknameBox);
	NicknameEdit = new QLineEdit(NicknameBox);
	GridLayout->addMultiCellWidget(NicknameBox, 0, 1, 0, 0);

	QVBox *NameBox = new QVBox(this);
	QLabel *NameLabel = new QLabel(tr("Name"), NameBox);
	NameEdit = new QLineEdit(NameBox);
	GridLayout->addMultiCellWidget(NameBox, 0, 1, 1, 1);
	
	QVBox *SurnameBox = new QVBox(this);
	QLabel *SurnameLabel = new QLabel(tr("Surname"), SurnameBox);
	SurnameEdit = new QLineEdit(SurnameBox);
	GridLayout->addMultiCellWidget(SurnameBox, 0, 1, 2, 2);

	QVBox *GenderBox = new QVBox(this);
	QLabel *GenderLabel = new QLabel(tr("Gender"), GenderBox);
	GenderCombo = new QComboBox(GenderBox);
	GenderCombo->insertItem("");	
	GenderCombo->insertItem(tr("Male"));
	GenderCombo->insertItem(tr("Female"));
	GridLayout->addMultiCellWidget(GenderBox, 2, 3, 0, 0);

	QVBox *BirthyearBox = new QVBox(this);
	QLabel *BirthyearLabel = new QLabel(tr("Birthyear"), BirthyearBox);
	BirthyearEdit = new QLineEdit(BirthyearBox);
	GridLayout->addMultiCellWidget(BirthyearBox, 2, 3, 1, 1);

	QVBox *CityBox = new QVBox(this);
	QLabel *CityLabel = new QLabel(tr("City"), CityBox);
	CityEdit = new QLineEdit(CityBox);
	GridLayout->addMultiCellWidget(CityBox, 2, 3, 2, 2);

	QVBox *FamilyNameBox = new QVBox(this);
	QLabel *FamilyNameLabel = new QLabel(tr("Family Name"), FamilyNameBox);
	FamilyNameEdit = new QLineEdit(FamilyNameBox);
	GridLayout->addMultiCellWidget(FamilyNameBox, 4, 5, 0, 0);

	QVBox *FamilyCityBox = new QVBox(this);
	QLabel *FamilyCityLabel = new QLabel(tr("Family City"), FamilyCityBox);
	FamilyCityEdit = new QLineEdit(FamilyCityBox);
	GridLayout->addMultiCellWidget(FamilyCityBox, 4, 5, 1, 1);

	QPushButton* OkButton = new QPushButton(tr("&OK"), this);
	GridLayout->addWidget(OkButton, 7, 1);
	connect(OkButton, SIGNAL(clicked()), this, SLOT(OkButtonClicked()));

	QPushButton* CancelButton = new QPushButton(tr("&Cancel"), this);
	GridLayout->addWidget(CancelButton, 7, 2);	
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	if (getActualStatus() != GG_STATUS_NOT_AVAIL) {
		gg_pubdir50_t req;
		req = gg_pubdir50_new(GG_PUBDIR50_READ);
		seq = gg_pubdir50(sess, req);
		connect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
			this, SLOT(fillFields(gg_pubdir50_t)));
		gg_pubdir50_free(req);
		State = READING;
		}
}

void PersonalInfoDialog::OkButtonClicked()
{
	if (getActualStatus() == GG_STATUS_NOT_AVAIL)
		return;

	char *nick, *first, *last, *city, *born, *family_name, *family_city;

	nick = NicknameEdit->text().length() ? strdup(unicode2cp(NicknameEdit->text()).data()) : NULL;
	first = NameEdit->text().length() ? strdup(unicode2cp(NameEdit->text()).data()) : NULL;
	last = SurnameEdit->text().length() ? strdup(unicode2cp(SurnameEdit->text()).data()) : NULL;
	city = CityEdit->text().length() ? strdup(unicode2cp(CityEdit->text()).data()) : NULL;
	born = BirthyearEdit->text().length() ? strdup(unicode2cp(BirthyearEdit->text()).data()) : NULL;
	family_name = FamilyNameEdit->text().length() ? strdup(unicode2cp(FamilyNameEdit->text()).data()) : NULL;
	family_city = FamilyCityEdit->text().length() ? strdup(unicode2cp(FamilyCityEdit->text()).data()) : NULL;

	gg_pubdir50_t req;
	req = gg_pubdir50_new(GG_PUBDIR50_WRITE);
	if (first) {
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)first);
		free(first);
		}
	if (last) {
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)last);
		free(last);
		}
	if (nick) {
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)nick);
		free(nick);
		}
	if (city) {
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)city);
		free(city);
		}
	if (GenderCombo->currentItem())
		gg_pubdir50_add(req, GG_PUBDIR50_GENDER, QString::number(GenderCombo->currentItem()).latin1());
	if (born) {
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)born);
		free(born);
		}
	if (family_name) {
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYNAME, (const char *)family_name);
		free(family_name);
		}
	if (family_city) {
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYCITY, (const char *)family_city);
		free(family_city);
		}

	seq = gg_pubdir50(sess, req);
	gg_pubdir50_free(req);
	connect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(fillFields(gg_pubdir50_t)));
	State = WRITTING;

	setEnabled(false);
}

void PersonalInfoDialog::fillFields(gg_pubdir50_t res)
{
	int count;
	const char *first, *last, *nick, *born, *city,
		*gender, *family_name, *family_city;

	if (res->seq != seq)
		return;

	disconnect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(fillFields(gg_pubdir50_t)));

	switch (State) {
		case READING:
			kdebug("PersonalInfoDialog::fillFields(): Done reading info,\n");

			first = gg_pubdir50_get(res, 0, GG_PUBDIR50_FIRSTNAME);
			last = gg_pubdir50_get(res, 0, GG_PUBDIR50_LASTNAME);
			nick = gg_pubdir50_get(res, 0, GG_PUBDIR50_NICKNAME);
			gender = gg_pubdir50_get(res, 0, GG_PUBDIR50_GENDER);
			born = gg_pubdir50_get(res, 0, GG_PUBDIR50_BIRTHYEAR);
			city = gg_pubdir50_get(res, 0, GG_PUBDIR50_CITY);
			family_name = gg_pubdir50_get(res, 0, GG_PUBDIR50_FAMILYNAME);
			family_city = gg_pubdir50_get(res, 0, GG_PUBDIR50_FAMILYCITY);
			if (first)
				NameEdit->setText(cp2unicode((unsigned char *)first));
			if (last)
				SurnameEdit->setText(cp2unicode((unsigned char *)last));
			if (nick)
				NicknameEdit->setText(cp2unicode((unsigned char *)nick));
			if (born && strcmp(born, "0"))
				BirthyearEdit->setText(cp2unicode((unsigned char *)born));
			if (city)
				CityEdit->setText(cp2unicode((unsigned char *)city));
			if (family_name)
				FamilyNameEdit->setText(cp2unicode((unsigned char *)family_name));
			if (family_city)
				FamilyCityEdit->setText(cp2unicode((unsigned char *)family_city));
			GenderCombo->setCurrentItem(gender ? atoi(gender) : 0);
			break;
		case WRITTING:
			kdebug("PersonalInfoDialog::fillFields(): Done writing info.\n");
			accept();
			break;
		}

	setEnabled(true);
}

void PersonalInfoDialog::closeEvent(QCloseEvent * e)
{
	disconnect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(fillFields(gg_pubdir50_t)));
	QWidget::closeEvent(e);
}

