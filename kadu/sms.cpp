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
#include <qregexp.h>
#include <qpainter.h>
#include <qurl.h>

#include "kadu.h"
#include "config_dialog.h"
#include "sms.h"
#include "history.h"
#include "debug.h"

/********** SmsImageWidget **********/

SmsImageWidget::SmsImageWidget(QWidget* parent,const QByteArray& image)
	: QWidget(parent, "SmsImageWidget"), Image(image)
{
	setCaption(i18n("Send SMS"));
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
	QLabel* label=new QLabel(i18n("Enter text from the picture:"),this);
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

HttpClient::HttpClient()
{
	connect(&Socket,SIGNAL(connected()),this,SLOT(onConnected()));
	connect(&Socket,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
};

void HttpClient::onConnected()
{
	QString query = (PostData.size() > 0 ? "POST" : "GET");
	query += " ";
	if (Path == "" || Path[0] != '/')
		query += '/';
	query += Path;
	query += " HTTP/1.1\n";
	query += "Host: " + Host + "\n";
	query += "User-Agent: Mozilla/5.0 (X11; U; Linux i686; pl-PL; rv:1.2)\n";
//	query+="Connection: keep-alive\n";
	if (Cookies.size() > 0) {
		query += "Cookie: ";

		QValueList<QString> keys;
		for (QMap<QString, QString>::const_iterator it = Cookies.begin(); it != Cookies.end(); ++it)
			keys.append(it.key());
		
//		wywolanie Cookies.keys() zostaje na lepsze czasu jak juz
//		wszyscy beda mieli Qt >= 3.0.5		
//    		for(int i=0; i<Cookies.keys().size(); i++)
		for (int i = 0; i < keys.size(); i++) {
			if (i > 0)
				query+="; ";
//			query+=Cookies.keys()[i]+"="+Cookies[Cookies.keys()[i]];
			query += keys[i] + "=" + Cookies[keys[i]];
			}
		query+="\n";
		}
	if (PostData.size() > 0) {
		query += "Content-Type: application/x-www-form-urlencoded\n";
		query += "Content-Length: " + QString::number(PostData.size()) + "\n";
		}
	query+="\n";
	if (PostData.size() > 0)
		query += QString(PostData);
	kdebug("HttpClient: Sending query:\n%s\n", query.local8Bit().data());
	Socket.writeBlock(query.local8Bit().data(), query.length());
};

void HttpClient::onReadyRead()
{
	int size=Socket.bytesAvailable();
	kdebug("HttpClient: Data Block Retreived: %i bytes\n",size);
	// Dodaj nowe dane do starych
	char buf[size];
	Socket.readBlock(buf,size);
	int old_size=Data.size();
	Data.resize(old_size+size);
	for(int i=0; i<size; i++)
		Data[old_size+i]=buf[i];
	// Jesli nie mamy jeszcze naglowka
	if(ContentLength<0)
	{	
		kdebug("HttpClient: Trying to parse header\n");
		// Kontynuuj odczyt jesli naglowek niekompletny
		QString s=QString(Data);
		int p=s.find("\r\n\r\n");
		if(p<0)
			return;
		// Dostalismy naglowek, 
		kdebug("HttpClient: Http header found:\n%s\n",s.local8Bit().data());		
		// Wyci±gamy status
		QRegExp status_regexp("HTTP/1\\.[01] (\\d+)");
		if(status_regexp.search(s)<0)
		{
			Socket.close();
			emit error();
			return;
		};
		Status=status_regexp.cap(1).toInt();
		kdebug("HttpClient: Status: %i\n",Status);			
		// Wyci±gamy Content-Length
		QRegExp cl_regexp("Content-Length: (\\d+)");
		if(cl_regexp.search(s)<0)
			ContentLength=0;
		else
			ContentLength=cl_regexp.cap(1).toInt();
		kdebug("HttpClient: Content-Length: %i bytes\n",ContentLength);			
		// Wyciagamy ewentualne cookie (dla uproszczenia tylko jedno)
		QRegExp cookie_regexp("Set-Cookie: ([^=]+)=([^;]+);");
		if(cookie_regexp.search(s)>=0)
		{
			QString cookie_name=cookie_regexp.cap(1);
			QString cookie_val=cookie_regexp.cap(2);
			Cookies.insert(cookie_name,cookie_val);
			kdebug("HttpClient: Cookie retreived: %s=%s\n",cookie_name.local8Bit().data(),cookie_val.local8Bit().data());
		};
		// Wytnij naglowek z Data
		int header_size=p+4;
		int new_data_size=Data.size()-header_size;
		for(int i=0; i<new_data_size; i++)
			Data[i]=Data[header_size+i];
		Data.resize(new_data_size);
		kdebug("HttpClient: Header parsed and cutted off from data\n");
		kdebug("HttpClient: Header size: %i bytes\n",header_size);
		kdebug("HttpClient: New data block size: %i bytes\n",new_data_size);
		// Je¶li status jest 100 - Continue to czekamy na dalsze dane
		// (uniewa¿niamy ten nag³owek i czekamy na nastêpny)
		if(Status==100)
		{
			ContentLength=-1;
			return;
		};
	};
	// Kontynuuj odczyt jesli dane niekompletne
	if(ContentLength>Data.size())
		return;
	// Mamy cale dane
	kdebug("HttpClient: All Data Retreived: %i bytes\n",Data.size());
	Socket.close();
	emit finished();
};

void HttpClient::setHost(QString host)
{
	Host=host;
	Cookies.clear();
};

void HttpClient::get(QString path)
{
	Path=path;
	Data.resize(0);
	PostData.resize(0);
	ContentLength=-1;
	Socket.connectToHost(Host,80);
};

void HttpClient::post(QString path,const QByteArray& data)
{
	Path=path;
	Data.resize(0);
	PostData.duplicate(data);
	ContentLength=-1;
	Socket.connectToHost(Host,80);
};

void HttpClient::post(QString path,const QString& data)
{
	QByteArray PostData;
	PostData.duplicate(data.local8Bit().data(),data.length());
	post(path,PostData);
};

int HttpClient::status()
{
	return Status;
};

const QByteArray& HttpClient::data()
{
	return Data;
};

QString HttpClient::encode(const QString& text)
{
	QString encoded=text;
	QUrl::encode(encoded);
	return encoded;
};

/********** SmsSender **********/

SmsSender::SmsSender(QObject* parent)
	: QObject(parent,"SmsSender")
{
	QObject::connect(&Http,SIGNAL(finished()),this,SLOT(onFinished()));
	QObject::connect(&Http,SIGNAL(error()),this,SLOT(onError()));
};

void SmsSender::onFinished()
{
	if(State==SMS_LOADING_PAGE)
	{
		QString Page=Http.data();
		kdebug("SMS Provider Page:\n%s\n",Page.local8Bit().data());
		if(Provider==SMS_IDEA)
		{
			QRegExp pic_regexp("rotate_vt\\.asp\\?token=([^\"]+)");
			int pic_pos=pic_regexp.search(Page);
			if(pic_pos<0)
			{
				QMessageBox::critical((QWidget*)parent(),"SMS",i18n("Provider gateway page looks strange. It's probably temporary disabled\nor has beed changed too much to parse it correctly."));
				emit finished(false);
				return;
			};
			QString pic_path=Page.mid(pic_pos,pic_regexp.matchedLength());
			Token=pic_regexp.cap(1);
			kdebug("SMS Idea Token: %s\n",Token.local8Bit().data());
			kdebug("SMS Idea Picture: %s\n",pic_path.local8Bit().data());
			State=SMS_LOADING_PICTURE;
			Http.get(pic_path);
		}
		else
		{
			QRegExp code_regexp("name=\\\"kod\\\" value=\\\"(\\d+)\\\"");
			QRegExp code_regexp2("name=\\\"Kod(\\d+)\\\" value=\\\"(\\d+)\\\"");
			if(code_regexp.search(Page) < 0) {
				QMessageBox::critical((QWidget*)parent(),"SMS",i18n("Provider gateway page looks strange. It's probably temporary disabled\nor has beed changed too much to parse it correctly."));
				emit finished(false);
				return;
				}
			if(code_regexp2.search(Page) < 0) {
				QMessageBox::critical((QWidget*)parent(),"SMS",i18n("Provider gateway page looks strange. It's probably temporary disabled\nor has beed changed too much to parse it correctly."));
				emit finished(false);
				return;
				}
			QString code = code_regexp.cap(1);
			QString num = code_regexp2.cap(1);
			QString code2 = code_regexp2.cap(2);
			State = SMS_LOADING_RESULTS;
			QString post_data = "bookopen=&numer="+Number+"&ksiazka=ksi%B1%BFka+telefoniczna&message="+Http.encode(Message)+"&podpis="+config.nick+"&kontakt=&Send=++tak-nada%E6++&Kod"+num+"="+code2+"&kod="+code;
			Http.post("sms/sendsms.asp", post_data);
		};
	}
	else if(State==SMS_LOADING_PICTURE)
	{
		kdebug("SMS Idea Picture Loaded: %i bytes\n",Http.data().size());
		SmsImageDialog* d=new SmsImageDialog((QDialog*)parent(),Http.data());
		connect(d,SIGNAL(codeEntered(const QString&)),this,SLOT(onCodeEntered(const QString&)));
		d->show();
	}
	else if(State==SMS_LOADING_RESULTS)
	{
		QString Page=Http.data();
		kdebug("SMS Provider Results Page:\n%s\n",Page.local8Bit().data());	
		if(Provider==SMS_IDEA)
		{
			if(Page.find("wyczerpany")>=0)
			{
				QMessageBox::critical((QWidget*)parent(),"SMS",i18n("You exceeded your daily limit"));
				emit finished(false);
			}
			else if(Page.find("wiadomo¶æ tekstowa zosta³a wys³ana")>=0)
			{
				emit finished(true);
			}
			else
			{
				QMessageBox::critical((QWidget*)parent(),"SMS",i18n("Provider gateway results page looks strange. SMS was probably NOT sent."));
				emit finished(false);
			};
		}
		else if(Provider==SMS_ERA)
		{
			if(Page.find("zosta³a wys³ana")>=0)
			{
				emit finished(true);
			}
			else
			{
				QMessageBox::critical((QWidget*)parent(),"SMS",i18n("Provider gateway results page looks strange. SMS was probably NOT sent."));
				emit finished(false);
			};		
		}
		else // SMS_PLUS
		{
			if(Page.find("SMS zosta³ wys³any")>=0)
			{
				emit finished(true);
			}
			else
			{
				QMessageBox::critical((QWidget*)parent(),"SMS",i18n("Provider gateway results page looks strange. SMS was probably NOT sent."));
				emit finished(false);
			};				
		};
	}
	else
		kdebug("SMS Panic! Unknown state\n");	
};

void SmsSender::onError()
{
	QMessageBox::critical((QWidget*)parent(),"SMS",i18n("Network error. Provider gateway page is probably unavailable"));
	emit finished(false);
};

void SmsSender::onCodeEntered(const QString& code)
{
	kdebug("SMS User entered the code\n");
	State=SMS_LOADING_RESULTS;
	QString post_data=QString("token=")+Token+"&SENDER="+config.nick+"&RECIPIENT="+Number+"&SHORT_MESSAGE="+Http.encode(Message)+"&pass="+code;
	Http.post("sendsms.asp",post_data);
};

void SmsSender::send(const QString& number,const QString& message)
{
	Number=number;
	Message=message;
	if(Number.length()==12&&Number.left(3)=="+48")
		Number=Number.right(9);
	if(Number.length()!=9)
	{
		QMessageBox::critical((QWidget*)parent(),"SMS",i18n("Mobile number is incorrect"));
		emit finished(false);
		return;
	};
	// Rozpoznaj siec
	if(Number[0]=='5')
		Provider=SMS_IDEA;
	else if(Number[0]=='6')
	{
		if((QChar(Number[2])-'0')%2)
			Provider=SMS_PLUS;
		else
 			Provider=SMS_ERA;
    	}
	else
	{
		QMessageBox::critical((QWidget*)parent(),"SMS",i18n("Mobile number is incorrect"));
		emit finished(false);
		return;
	};	
	// Wyslij
	if(Provider==SMS_IDEA)
	{
		State=SMS_LOADING_PAGE;
		Http.setHost("213.218.116.131");
		Http.get("/");
	}
	else if(Provider==SMS_ERA)
	{
		State=SMS_LOADING_PAGE;
		Http.setHost("213.158.194.32");
		Http.post("sms/sendsms.asp","sms=1");
	}
	else
	{
		State=SMS_LOADING_RESULTS;
		Http.setHost("212.2.96.57");
		QString post_data="tprefix="+Number.left(3)+"&numer="+Number.right(6)+"&odkogo="+config.nick+"&tekst="+Message;
		Http.post("sms/sendsms.php",post_data);
	};
};

/********** Sms **********/

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

	QLabel *recilabel = new QLabel(i18n("Recipient"),this);
	grid->addWidget(recilabel, 0, 0);

	b_send = new QPushButton(this);
	b_send->setText(i18n("Send"));
	grid->addWidget(b_send, 3, 3);
	QObject::connect(b_send, SIGNAL(clicked()), this, SLOT(sendSms()));

	smslen = new QLabel("0",this);
	grid->addWidget(smslen, 3, 0);

	resize(300,200);
	setCaption(i18n("Send SMS"));

	connect(&Sender,SIGNAL(finished(bool)),this,SLOT(onSmsSenderFinished(bool)));
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

	history.appendSms(recipient->text(), body->text());

	if(config.smsbuildin)
	{
		Sender.send(recipient->text(),body->text());
	}
	else
	{
		if(config.smsapp=="")
		{
			QMessageBox::warning(this, i18n("SMS error"), i18n("Sms application was not specified. Visit the configuration section") );
			kdebug("SMS application NOT specified. Exit.\n");
			return;
		};
		QString SmsAppPath=config.smsapp;
		
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
	};
}

void Sms::smsSigHandler() {
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

void Sms::onSmsSenderFinished(bool success)
{
	if(success)
		QMessageBox::information(this, i18n("SMS sent"), i18n("The SMS was sent and should be on its way"));
	b_send->setEnabled(true);
	body->setEnabled(true);
	if(success)
		body->clear();
};

