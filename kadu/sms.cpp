/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlayout.h>
#include <qmessagebox.h>
#include <klocale.h>
#include <qregexp.h>
#include <qpainter.h>
//
#include "kadu.h"
#include "sms.h"
#include "history.h"
//

Sms::Sms(const QString& altnick, QDialog* parent) : QDialog (parent, "Sms")
{
	QGridLayout * grid = new QGridLayout(this, 3, 4, 10, 3);

	body = new QMultiLineEdit(this);
	grid->addMultiCellWidget(body, 1, 1, 0, 3);
	body->setWordWrap(QMultiLineEdit::WidgetWidth);
	body->setFont(config.fonts.chat);
	QObject::connect(body, SIGNAL(textChanged()), this, SLOT(updateCounter()));

	recipient = new QLineEdit(this);
	if(altnick!="")
		recipient->setText(userlist.byAltNick(altnick).mobile);
	QObject::connect(recipient,SIGNAL(textChanged(const QString&)),this,SLOT(updateList(const QString&)));
	grid->addWidget(recipient, 0, 1);

	list = new QComboBox(this);
	list->insertItem("");
	for(int i=0; i<userlist.count(); i++)
	{
		if (userlist[i].mobile.length())
			list->insertItem(userlist[i].altnick);
	};
	list->setCurrentText(altnick);
	QObject::connect(list, SIGNAL(activated(const QString&)), this, SLOT(updateRecipient(const QString &)));
	grid->addWidget(list, 0, 3);

	QLabel *recilabel = new QLabel(this);
	recilabel->setText(i18n("Recipient"));
	grid->addWidget(recilabel, 0, 0);

	b_send = new QPushButton(this);
	b_send->setText(i18n("Send"));
	grid->addWidget(b_send, 3, 3);
	QObject::connect(b_send, SIGNAL(clicked()), this, SLOT(sendSms()));

	QPushButton* b_send_int = new QPushButton(this);
	b_send_int->setText(i18n("Send (internal)"));
	grid->addWidget(b_send_int, 3, 1);
	QObject::connect(b_send_int, SIGNAL(clicked()), this, SLOT(sendSmsInternal()));

	smslen = new QLabel(this);
	smslen->setText("0");
	grid->addWidget(smslen, 3, 0);

	resize(300,200);
	setCaption(i18n("Send SMS"));
}

void Sms::updateRecipient(const QString &newtext)
{
	if(newtext=="")
	{
		recipient->setText("");
		return;
	};
	for(int i=0; i<userlist.count(); i++)
		if(userlist[i].altnick==newtext)
		{
			recipient->setText(userlist[i].mobile);
			break;
		};
}

void Sms::updateList(const QString &newnumber)
{
	for(int i=0; i<userlist.count(); i++)
		if(userlist[i].mobile==newnumber)
		{
			list->setCurrentText(userlist[i].altnick);
			return;
		};
	list->setCurrentText("");
};

void Sms::sendSms(void) {
	b_send->setEnabled(false);
	body->setEnabled(false);

	QString SmsAppPath;
	if(config.smsbuildin)
		SmsAppPath=QString(BINDIR)+"/kadusms";
	else
	{
		if(config.smsapp=="")
		{
			QMessageBox::warning(this, i18n("SMS error"), i18n("Sms application was not specified. Visit the configuration section") );
			fprintf(stderr,"KK SMS application NOT specified. Exit.\n");
			return;
		};
		SmsAppPath=config.smsapp;
	};
		
	smsProcess = new QProcess(this);
	if(config.smscustomconf&&(!config.smsbuildin))
	{
		QStringList args=QStringList::split(' ',config.smsconf);
		if(args.find("%n")!=args.end())
			*args.find("%n")=recipient->text();
		if(args.find("%m")!=args.end())
			*args.find("%m")=body->text();
		args.prepend(SmsAppPath);
		smsProcess->setArguments(args);
	}
	else
	{
		smsProcess->addArgument(SmsAppPath);
		smsProcess->addArgument(recipient->text());
		smsProcess->addArgument(body->text());
	};

	if (!smsProcess->start())
		QMessageBox::critical(this, i18n("SMS error"), i18n("Could not spawn child process. Check if the program is functional") );
	QObject::connect(smsProcess, SIGNAL(processExited()), this, SLOT(smsSigHandler()));
}

void Sms::smsSigHandler() {
	appendSMSHistory(recipient->text(),body->text());
	if (smsProcess->normalExit())
		QMessageBox::information(this, i18n("SMS sent"), i18n("The process exited normally. The SMS should be on its way"));
	else
		QMessageBox::warning(this, i18n("SMS not sent"), i18n("The process exited abnormally. The SMS may not be sent"));
	b_send->setEnabled(true);
	body->setEnabled(true);
	body->clear();
}

void Sms::updateCounter() {
	char len[10];
	snprintf(len, sizeof(len), "%d", body->text().length());
	smslen->setText(len);
}

void Sms::sendSmsInternal()
{
	(new SmsThread(this,"502387781","xxx"))->start();
};

/********** SmsImageWidget **********/

SmsImageWidget::SmsImageWidget(QWidget* parent,const QByteArray& image)
	: QWidget(parent, "SmsImageWidget"), Image(image)
{
	setMinimumSize(Image.width(),Image.height());
};

void SmsImageWidget::paintEvent(QPaintEvent* e)
{
	QPainter p(this);
	p.drawImage(0,0,Image);
};

/********** SmsImageDialog **********/

SmsImageDialog::SmsImageDialog(QDialog* parent,const QByteArray& image)
	: QDialog (parent, "SmsImageDialog")
{
	QGridLayout * grid = new QGridLayout(this, 2, 2, 10, 10);
	SmsImageWidget* image_widget=new SmsImageWidget(this,image);
	grid->addMultiCellWidget(image_widget, 0, 0, 0, 1);
	QLabel* label=new QLabel(this);
	label->setText(i18n("Enter text from the picture:"));
	grid->addWidget(label, 1, 0);
	code_edit=new QLineEdit(this);
	grid->addWidget(code_edit, 1, 1);
	connect(code_edit,SIGNAL(returnPressed()),this,SLOT(onReturnPressed()));
};

void SmsImageDialog::onReturnPressed()
{
	accept();
	emit codeEntered(code_edit->text());
};

/********** SmsThread **********/

SmsThread::SmsThread(QObject* parent,const QString& number,const QString& message)
	: QObject(parent,"SmsThread"), QThread()
{
	qInitNetworkProtocols();
};

void SmsThread::onFinished(QNetworkOperation* op)
{
	fprintf(stderr,"SMS Operation State: %i\n",op->state());
	if(State==SMS_LOADING_PAGE)
	{
		QString Page=QString(Data);
		fprintf(stderr,"SMS Idea Page:\n%s\n",Page.local8Bit().data());
		QRegExp pic_regexp("rotate_vt\\.asp\\?token=[^\"]+");
		int pic_pos=pic_regexp.search(Page);
		QString pic_path;
		if(pic_pos>-1)
			pic_path=Page.mid(pic_pos,pic_regexp.matchedLength());
		fprintf(stderr,"SMS Idea Picture: %s\n",pic_path.local8Bit().data());
		State=SMS_LOADING_PICTURE;
		Data.resize(0);
		delete UrlOp;
		UrlOp=new QUrlOperator("http://213.218.116.131/"+pic_path);
		QObject::connect(UrlOp,SIGNAL(finished(QNetworkOperation*)),this,SLOT(onFinished(QNetworkOperation*)));
		QObject::connect(UrlOp,SIGNAL(data(const QByteArray&, QNetworkOperation*)),this,SLOT(onData(const QByteArray&, QNetworkOperation*)));	
		UrlOp->get();
	}
	else if(State=SMS_LOADING_PICTURE)
	{
		fprintf(stderr,"SMS Idea Picture Loaded: %i bytes\n",Data.size());
		delete UrlOp;
		SmsImageDialog* d=new SmsImageDialog((QDialog*)parent(),Data);
		connect(d,SIGNAL(codeEntered(const QString&)),this,SLOT(onCodeEntered(const QString&)));
		d->show();
	}
	else
	{
//		delete UrlOp;
	};
};

void SmsThread::onCodeEntered(const QString& code)
{
	State=SMS_LOADING_RESULTS;
/*	UrlOp=new QUrlOperator(QString("http://213.218.116.131/default.asp?"));
	QObject::connect(UrlOp,SIGNAL(finished(QNetworkOperation*)),this,SLOT(onFinished(QNetworkOperation*)));
	QObject::connect(UrlOp,SIGNAL(data(const QByteArray&, QNetworkOperation*)),this,SLOT(onData(const QByteArray&, QNetworkOperation*)));
	UrlOp->get();	*/
};

void SmsThread::onData(const QByteArray& data,QNetworkOperation* op)
{
	int old_size=Data.size();
	Data.resize(old_size+data.size());
	for(int i=0; i<data.size(); i++)
		Data[old_size+i]=data[i];
};

void SmsThread::run()
{
	State=SMS_LOADING_PAGE;
	UrlOp=new QUrlOperator("http://213.218.116.131");
	QObject::connect(UrlOp,SIGNAL(finished(QNetworkOperation*)),this,SLOT(onFinished(QNetworkOperation*)));
	QObject::connect(UrlOp,SIGNAL(data(const QByteArray&, QNetworkOperation*)),this,SLOT(onData(const QByteArray&, QNetworkOperation*)));	
	UrlOp->get();
};

#include "sms.moc"
