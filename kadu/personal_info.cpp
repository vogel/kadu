/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlabel.h>
#include <qmessagebox.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include <stdlib.h>

#include "gadu.h"
#include "status.h"
#include "debug.h"
#include "events.h"
#include "personal_info.h"

PersonalInfoDialog::PersonalInfoDialog(QDialog *parent, const char *name)
{
	kdebugf();
	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("Personal Information"));
	
	// create main QLabel widgets (icon and app info)
	QVBox *left=new QVBox(this);
	left->setMargin(10);
	left->setSpacing(10);
	
	QLabel *l_icon = new QLabel(left);
	QWidget *blank=new QWidget(left);
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
	
	QVBox *center=new QVBox(this);
	center->setMargin(10);
	center->setSpacing(10);
	
	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager.loadIcon("PersonalInformationWindowIcon"));
	l_info->setText(tr("This dialog box allows you to manage your personal information, for example your "
				"name, surname or age."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)
	
	// our QVGroupBox
	QVGroupBox *vgb_info = new QVGroupBox(center);
	vgb_info->setTitle(tr("Your personal information"));
	center->setStretchFactor(vgb_info, 1);
	// end our QGroupBox
	
	// create our info-fields
	QHBox *hb_nicknamesurname = new QHBox(vgb_info);
	QVBox *vb_nickname = new QVBox(hb_nicknamesurname);
	QVBox *vb_name = new QVBox(hb_nicknamesurname);
	QVBox *vb_surname = new QVBox(hb_nicknamesurname);
	hb_nicknamesurname->setSpacing(3);
	vb_nickname->setSpacing(3);
	vb_name->setSpacing(3);
	vb_surname->setSpacing(3);
	
	new QLabel(tr("Nickname"), vb_nickname);
	le_nickname = new QLineEdit(vb_nickname);
	new QLabel(tr("Name"), vb_name);
	le_name = new QLineEdit(vb_name);
	new QLabel(tr("Surname"), vb_surname);
	le_surname = new QLineEdit(vb_surname);
	
	//
	QHBox *hb_genderbirthyearcity = new QHBox(vgb_info);
	QVBox *vb_gender = new QVBox(hb_genderbirthyearcity);
	QVBox *vb_birthyear = new QVBox(hb_genderbirthyearcity);
	QVBox *vb_city = new QVBox(hb_genderbirthyearcity);
	hb_genderbirthyearcity->setSpacing(3);
	vb_gender->setSpacing(3);
	vb_birthyear->setSpacing(3);
	vb_city->setSpacing(3);
	
	new QLabel(tr("Gender"), vb_gender);
	cb_gender = new QComboBox(vb_gender);
	cb_gender->insertItem("");	
	cb_gender->insertItem(tr("Male"));
	cb_gender->insertItem(tr("Female"));
	cb_gender->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	hb_genderbirthyearcity->setStretchFactor(cb_gender, 1);
	new QLabel(tr("Birthyear"), vb_birthyear);
	le_birthyear = new QLineEdit(vb_birthyear);
	hb_genderbirthyearcity->setStretchFactor(le_birthyear, 1);
	new QLabel(tr("City"), vb_city);
	le_city = new QLineEdit(vb_city);
	hb_genderbirthyearcity->setStretchFactor(le_city, 1);
	
	//
	QHBox *hb_fnamefcity = new QHBox(vgb_info);
	QVBox *vb_fname = new QVBox(hb_fnamefcity);
	QVBox *vb_fcity = new QVBox(hb_fnamefcity);
	QVBox *vb_blank = new QVBox(hb_fnamefcity);
	hb_fnamefcity->setSpacing(3);
	vb_fname->setSpacing(3);
	vb_fcity->setSpacing(3);
	vb_blank->setSpacing(3);
	
	new QLabel(tr("Family Name"), vb_fname);
	le_familyname = new QLineEdit(vb_fname);
	new QLabel(tr("Family City"), vb_fcity);
	le_familycity = new QLineEdit(vb_fcity);
	QWidget *w_blankfill = new QWidget(vb_blank);
	w_blankfill->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	//
	
	// buttons
	QHBox *bottom=new QHBox(center);
	QWidget *blank2=new QWidget(bottom);
	bottom->setSpacing(5);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	pb_save = new QPushButton(icons_manager.loadIcon("SavePersonalInfoButton"), tr("&Save"), bottom, "save");
	QPushButton *pb_reload = new QPushButton(icons_manager.loadIcon("ReloadPersonalInfoButton"), tr("&Reload"), bottom, "save");
	QPushButton *pb_close = new QPushButton(icons_manager.loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons
	
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(pb_save, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));
	connect(pb_reload, SIGNAL(clicked()), this, SLOT(reloadInfo()));
	
	reloadInfo();
	
	kdebugf2();
 	loadGeometry(this, "General", "PersonalInfoDialogGeometry", 0, 0, 450, 400);
}

PersonalInfoDialog::~PersonalInfoDialog()
{
	kdebugf();
	saveGeometry(this, "General", "PersonalInfoDialogGeometry");
}

void PersonalInfoDialog::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
	if ((ke_event->key() == Qt::Key_Return || ke_event->key() == Qt::Key_Enter))
		saveButtonClicked();
}

void PersonalInfoDialog::reloadInfo()
{
	if (getCurrentStatus() != GG_STATUS_NOT_AVAIL) 
	{
		gg_pubdir50_t req;
		req = gg_pubdir50_new(GG_PUBDIR50_READ);
		seq = gg_pubdir50(sess, req);
		connect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
			this, SLOT(fillFields(gg_pubdir50_t)));
		gg_pubdir50_free(req);
		pb_save->setEnabled(true);
		State = READING;
	}
	else
		pb_save->setEnabled(false);
}

void PersonalInfoDialog::saveButtonClicked()
{
	kdebugf();
	if (getCurrentStatus() == GG_STATUS_NOT_AVAIL)
		return;

	char *nick, *first, *last, *city, *born, *family_name, *family_city;

	nick = le_nickname->text().length() ? strdup(unicode2cp(le_nickname->text()).data()) : NULL;
	first = le_name->text().length() ? strdup(unicode2cp(le_name->text()).data()) : NULL;
	last = le_surname->text().length() ? strdup(unicode2cp(le_surname->text()).data()) : NULL;
	city = le_city->text().length() ? strdup(unicode2cp(le_city->text()).data()) : NULL;
	born = le_birthyear->text().length() ? strdup(unicode2cp(le_birthyear->text()).data()) : NULL;
	family_name = le_familyname->text().length() ? strdup(unicode2cp(le_familyname->text()).data()) : NULL;
	family_city = le_familycity->text().length() ? strdup(unicode2cp(le_familycity->text()).data()) : NULL;

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
	if (cb_gender->currentItem())
		gg_pubdir50_add(req, GG_PUBDIR50_GENDER, QString::number(cb_gender->currentItem()).latin1());
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
	kdebugf2();
}

void PersonalInfoDialog::fillFields(gg_pubdir50_t res)
{
	kdebugf();
//	int count;
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
				le_name->setText(cp2unicode((unsigned char *)first));
			if (last)
				le_surname->setText(cp2unicode((unsigned char *)last));
			if (nick)
				le_nickname->setText(cp2unicode((unsigned char *)nick));
			if (born && strcmp(born, "0"))
				le_birthyear->setText(cp2unicode((unsigned char *)born));
			if (city)
				le_city->setText(cp2unicode((unsigned char *)city));
			if (family_name)
				le_familyname->setText(cp2unicode((unsigned char *)family_name));
			if (family_city)
				le_familycity->setText(cp2unicode((unsigned char *)family_city));
			cb_gender->setCurrentItem(gender ? atoi(gender) : 0);
			break;
		case WRITTING:
			kdebug("PersonalInfoDialog::fillFields(): Done writing info.\n");
			close();
			break;
		case READY:
			break;
		}

	setEnabled(true);
	kdebugf2();
}

void PersonalInfoDialog::closeEvent(QCloseEvent * e)
{
	disconnect(&event_manager, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
		this, SLOT(fillFields(gg_pubdir50_t)));
	QWidget::closeEvent(e);
}

