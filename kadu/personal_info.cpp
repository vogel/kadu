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
	GenderCombo->insertItem(i18n("Female"));
	GenderCombo->insertItem(i18n("Male"));
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

	http=gg_search(gg_search_request_mode_3(config.uin,FALSE,0),TRUE);
	if(http==NULL)
	{
		QMessageBox::critical(this,i18n("Error"),i18n("Public directory read failed"));
		return;	
	};
	
	setEnabled(false);
	State=READING;
	createSocketNotifiers();
};

void PersonalInfoDialog::OkButtonClicked()
{
	char* nick = strdup(NicknameEdit->text().local8Bit());
	char* firstname = strdup(NameEdit->text().local8Bit());
	char* lastname = strdup(SurnameEdit->text().local8Bit());
	char* city = strdup(CityEdit->text().local8Bit());
    
	iso_to_cp((unsigned char *)nick);
	iso_to_cp((unsigned char *)firstname);
	iso_to_cp((unsigned char *)lastname);
	iso_to_cp((unsigned char *)city);

	struct gg_change_info_request* request=
		gg_change_info_request_new(firstname,lastname,
			nick,"",BirthyearEdit->text().toUInt(),
			GenderCombo->currentItem(),city);
	if(request==NULL)
	{
		QMessageBox::critical(this,i18n("Error"),i18n("Memory allocation error"));
		return;	
	};

	http=gg_change_info(config.uin,config.password,request,TRUE);
	if(http==NULL)
	{
		QMessageBox::critical(this,i18n("Error"),i18n("Public directory write failed"));
		gg_change_info_request_free(request);		
		return;	
	};
	gg_change_info_request_free(request);

	setEnabled(false);

	delete nick;
	delete firstname;
	delete lastname;
	delete city;

	State=WRITTING;
	createSocketNotifiers();		
};

void PersonalInfoDialog::dataReceived()
{
    if (http->check & GG_CHECK_READ)
	socketEvent();
}

void PersonalInfoDialog::dataSent()
{
    if (http->check & GG_CHECK_WRITE)
	socketEvent();
}

void PersonalInfoDialog::socketEvent()
{
	fprintf(stderr,"PersonalInfoDialog::socketEvent()\n");

	int res;
	if(State == WRITTING)
		res = gg_pubdir_watch_fd(http);
	else
		res = gg_search_watch_fd(http);
		
	if(res<0) {
		setEnabled(true);
		deleteSocketNotifiers();
		if (State == WRITTING) {
			QMessageBox::critical(this,i18n("Error"),i18n("Public directory write failed"));
			gg_pubdir_free(http);
			}
		else {
			QMessageBox::critical(this,i18n("Error"),i18n("Public directory read failed"));		
			gg_free_search(http);
			}
		State = READY;
		return;
		}

	if (http->state == GG_STATE_CONNECTING) {
		fprintf(stderr, "KK PersonalInfoDialog::socketEvent(): changing QSocketNotifiers.\n");
		deleteSocketNotifiers();
		createSocketNotifiers();
		}

	if (http->state == GG_STATE_ERROR) {
		setEnabled(true);
		deleteSocketNotifiers();
		if (State == WRITTING) {
			QMessageBox::critical(this,i18n("Error"),i18n("Public directory write failed"));
			gg_pubdir_free(http);
			}
		else {
			QMessageBox::critical(this,i18n("Error"),i18n("Public directory read failed"));		
			gg_free_search(http);
			}
		State = READY;
		return;
		}

	if (http->state == GG_STATE_DONE) {
		setEnabled(true);
		deleteSocketNotifiers();
		if (State == WRITTING) {
			gg_pubdir_free(http);
			accept();
			}
		else {
			fillFields();
			gg_free_search(http);
			}	
		State = READY;
		return;
		}
}

void PersonalInfoDialog::createSocketNotifiers()
{
	fprintf(stderr,"PersonalInfoDialog::createSocketNotifiers()\n");

	SocketReadNotifier = new QSocketNotifier(http->fd, QSocketNotifier::Read);
    	connect(SocketReadNotifier, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	SocketWriteNotifier = new QSocketNotifier(http->fd, QSocketNotifier::Write);
	connect(SocketWriteNotifier, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void PersonalInfoDialog::deleteSocketNotifiers()
{
	SocketReadNotifier->setEnabled(false);
	delete SocketReadNotifier;
	SocketWriteNotifier->setEnabled(false);
	delete SocketWriteNotifier;
}

void PersonalInfoDialog::fillFields()
{
	struct gg_search_result* search_res=((struct gg_search*)http->data)->results;

	cp_to_iso((unsigned char *)search_res->nickname);
	cp_to_iso((unsigned char *)search_res->first_name);
	cp_to_iso((unsigned char *)search_res->last_name);
	cp_to_iso((unsigned char *)search_res->city);
	
	NicknameEdit->setText(__c2q(search_res->nickname));
	NameEdit->setText(__c2q(search_res->first_name));
	SurnameEdit->setText(__c2q(search_res->last_name));
	GenderCombo->setCurrentItem(search_res->gender);
	QCString BirthyearString;
	BirthyearString.setNum(search_res->born);
	BirthyearEdit->setText(BirthyearString);
	CityEdit->setText(__c2q(search_res->city));
};

void PersonalInfoDialog::closeEvent(QCloseEvent * e)
{
	if (State == WRITTING) {
		gg_pubdir_free(http);
		deleteSocketNotifiers();
		}
	else
		if (State == READING) {
			gg_free_search(http);
			deleteSocketNotifiers();
			}
	QWidget::closeEvent(e);
}

#include "personal_info.moc"
