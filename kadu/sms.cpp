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
	(new SmsSender(this,"502387781","xxx"))->run();
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

/********** HttpClient **********/

HttpClient::HttpClient(QString host)
{
	Host=host;
	connect(this,SIGNAL(connected()),this,SLOT(onConnected()));
	connect(this,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
};

void HttpClient::onConnected()
{
	QString query=(PostData.size()>0?"POST":"GET");
	query+=" ";
	query+=Path;
	query+=" HTTP/1.1\n";
	query+="Host: "+Host+"\n";
	query+="User-Agent: Mozilla/5.0 (X11; U; Linux i686; pl-PL; rv:1.2)\n";
	query+="Connection: keep-alive\n";
	if(CookieName!="")
		query+="Cookie: "+CookieName+"="+CookieValue+"\n";
	if(PostData.size()>0)
	{
		query+="Content-Type: application/x-www-form-urlencoded\n";
		query+="Content-Length: "+QString::number(PostData.size())+"\n";
	};
	query+="\n";
	if(PostData.size()>0)
		query+=QString(PostData);
	fprintf(stderr,"HttpClient: Sending query:\n%s\n",query.local8Bit().data());	
	writeBlock(query.local8Bit().data(),query.length());
};

void HttpClient::onReadyRead()
{
	int size=bytesAvailable();
	fprintf(stderr,"HttpClient: Data Block Retreived: %i bytes\n",size);
	// Dodaj nowe dane do starych
	char buf[size];
	readBlock(buf,size);
	int old_size=Data.size();
	Data.resize(old_size+size);
	for(int i=0; i<size; i++)
		Data[old_size+i]=buf[i];
	// Jesli nie mamy jeszcze naglowka
	if(ContentLength<0)
	{	
		fprintf(stderr,"HttpClient: Trying to parse header\n");
		// Kontynuuj odczyt jesli naglowek niekompletny
		QString s=QString(Data);
		int p=s.find("\r\n\r\n");
		if(p<0)
			return;
		// Dostalismy naglowek, pobieramy Content-Length
		fprintf(stderr,"HttpClient: Http header found\n");
		QRegExp cl_regexp("Content-Length: (\\d+)");
		if(cl_regexp.search(s)<0)
			ContentLength=0;
		else
			ContentLength=atoi(cl_regexp.cap(1).local8Bit().data());
		fprintf(stderr,"HttpClient: Content-Length: %i bytes\n",ContentLength);			
		// Wyciagamy ewentualne cookie (dla uproszczenia tylko jedno)
		QRegExp cookie_regexp("Set-Cookie: ([^=]+)=([^;]+);");
		if(cookie_regexp.search(s)>=0)
		{
			CookieName=cookie_regexp.cap(1);
			CookieValue=cookie_regexp.cap(2);
			fprintf(stderr,"HttpClient: Cookie retreived: %s=%s\n",CookieName.local8Bit().data(),CookieValue.local8Bit().data());
		};
		// Wytnij naglowek z Data
		int header_size=p+4;
		int new_data_size=Data.size()-header_size;
		for(int i=0; i<new_data_size; i++)
			Data[i]=Data[header_size+i];
		Data.resize(new_data_size);
		fprintf(stderr,"HttpClient: Header parsed and cutted off from data\n");
		fprintf(stderr,"HttpClient: Header size: %i bytes\n",header_size);
		fprintf(stderr,"HttpClient: New data block size: %i bytes\n",new_data_size);
	};
	// Kontynuuj odczyt jesli dane niekompletne
	if(ContentLength>Data.size())
		return;
	// Mamy cale dane
	fprintf(stderr,"HttpClient: All Data Retreived: %i bytes\n",Data.size());
	close();
	emit finished();
};

void HttpClient::get(QString path)
{
	Path=path;
	Data.resize(0);
	PostData.resize(0);
	ContentLength=-1;
	connectToHost(Host,80);
};

void HttpClient::post(QString path,const QByteArray& data)
{
	Path=path;
	Data.resize(0);
	PostData.duplicate(data);
	ContentLength=-1;
	connectToHost(Host,80);
};

const QByteArray& HttpClient::data()
{
	return Data;
};

/********** SmsSender **********/

SmsSender::SmsSender(QObject* parent,const QString& number,const QString& message)
	: QObject(parent,"SmsSender")
{
	Number=number;
	Message=message;
	Http=new HttpClient("213.218.116.131");
	QObject::connect(Http,SIGNAL(finished()),this,SLOT(onFinished()));
};

void SmsSender::onFinished()
{
	//fprintf(stderr,"SMS Operation State: %i\n",op->state());
	if(State==SMS_LOADING_PAGE)
	{
		QString Page=Http->data();
		fprintf(stderr,"SMS Idea Page:\n%s\n",Page.local8Bit().data());
		QRegExp pic_regexp("rotate_vt\\.asp\\?token=([^\"]+)");
		int pic_pos=pic_regexp.search(Page);
		if(pic_pos<0)
		{
			QMessageBox::critical((QWidget*)parent(),"SMS",i18n("Provider gateway page looks strange. It's probably temporary disabled\nor has beed changed too much to parse it correctly."));
			return;
		};
		QString pic_path=Page.mid(pic_pos,pic_regexp.matchedLength());
		Token=pic_regexp.cap(1);
		fprintf(stderr,"SMS Idea Token: %s\n",Token.local8Bit().data());
		fprintf(stderr,"SMS Idea Picture: %s\n",pic_path.local8Bit().data());
		State=SMS_LOADING_PICTURE;
		Http->get(QString("/")+pic_path);
	}
	else if(State==SMS_LOADING_PICTURE)
	{
		fprintf(stderr,"SMS Idea Picture Loaded: %i bytes\n",Http->data().size());
		SmsImageDialog* d=new SmsImageDialog((QDialog*)parent(),Http->data());
		connect(d,SIGNAL(codeEntered(const QString&)),this,SLOT(onCodeEntered(const QString&)));
		d->show();
	}
	else if(State==SMS_LOADING_RESULTS)
	{
		QString Page=Http->data();
		fprintf(stderr,"SMS Idea Results Page:\n%s\n",Page.local8Bit().data());	
	}
	else
		fprintf(stderr,"SMS Panic! Unknown state\n");	
};

void SmsSender::onCodeEntered(const QString& code)
{
	fprintf(stderr,"SMS User entered the code\n");
	State=SMS_LOADING_RESULTS;
	QString post_data=QString("token=")+Token+"&SENDER=Kadu&RECIPIENT="+Number+"&SHORT_MESSAGE="+Message+"&pass="+code;
	QByteArray PostData;
	PostData.duplicate(post_data.local8Bit().data(),post_data.length());
	Http->post("/sendsms.asp",PostData);
};

void SmsSender::run()
{
	State=SMS_LOADING_PAGE;
	Http->get("/");
};

#include "sms.moc"
