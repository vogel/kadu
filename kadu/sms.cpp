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
#include <qgrid.h>

#include "config_dialog.h"
#include "config_file.h"
#include "sms.h"
#include "history.h"
#include "debug.h"

/********** SmsImageDialog **********/

SmsImageDialog::SmsImageDialog(QDialog* parent,const QByteArray& image)
	: QDialog (parent, "SmsImageDialog")
{
	QGridLayout *grid = new QGridLayout(this, 2, 2, 10, 10);
	ImageWidget *image_widget = new ImageWidget(this, image);
	grid->addMultiCellWidget(image_widget, 0, 0, 0, 1);
	QLabel* label=new QLabel(tr("Enter text from the picture:"),this);
	grid->addWidget(label, 1, 0);
	code_edit=new QLineEdit(this);
	grid->addWidget(code_edit, 1, 1);
	connect(code_edit,SIGNAL(returnPressed()),this,SLOT(onReturnPressed()));
};

void SmsImageDialog::reject()
{
	emit codeEntered("");
	QDialog::reject();
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
	connect(&Socket,SIGNAL(connectionClosed()),this,SLOT(onConnectionClosed()));
};

void HttpClient::onConnected()
{
	QString query = (PostData.size() > 0 ? "POST" : "GET");
	query += " ";
	if ((Path == "" || Path[0] != '/')&&Path.left(7)!="http://")
		query += '/';
	query += Path;
	query += " HTTP/1.1\r\n";
	query += "Host: " + Host + "\r\n";
	query += "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.4) Gecko/20030617\r\n";
//	query += "Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8\r\n";
//	query += "Connection: keep-alive\r\n";
	if(Referer!="")
		query += "Referer: "+Referer+"\r\n";
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
		query+="\r\n";
		}
	if (PostData.size() > 0) {
		query += "Content-Type: application/x-www-form-urlencoded\r\n";
		query += "Content-Length: " + QString::number(PostData.size()) + "\r\n";
		}
	query+="\r\n";
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
	//
//	kdebug("%s\n",buf);
	//
	int old_size=Data.size();
	Data.resize(old_size+size);
	for(int i=0; i<size; i++)
		Data[old_size+i]=buf[i];
	// Jesli nie mamy jeszcze naglowka
	if(!HeaderParsed)
	{	
		kdebug("HttpClient: Trying to parse header\n");
		// Kontynuuj odczyt jesli naglowek niekompletny
		QString s=QString(Data);
		int p=s.find("\r\n\r\n");
		if(p<0)
			return;
		// Dostalismy naglowek, 
		kdebug("HttpClient: Http header found:\n%s\n",s.local8Bit().data());		
		HeaderParsed=true;
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
		// Status 302 oznacza przekierowanie.
		if(Status==302)
		{
			QRegExp location_regexp("Location: ([^\\r\\n]+)");
			if(location_regexp.search(s)<0)
			{
				Socket.close();
				emit error();
				return;
			};
			QString location=location_regexp.cap(1);
			kdebug("Jumping to %s\n",location.local8Bit().data());
			// czekamy. zbyt szybkie przekierowanie konczy sie
			// czasem petla. nie wiem dlaczego.
			QTime* t=new QTime();
			t->start();
			while (t->elapsed()<500) {};
			delete t;
			//
			emit redirected(location);
			get(location);
			return;
		};
		// Wyci±gamy Content-Length
		QRegExp cl_regexp("Content-Length: (\\d+)");
		if(cl_regexp.search(s)<0)
		{
			ContentLength=-1;
			kdebug("HttpClient: Content-Length not found. We will wait for connection to close.");
		}
		else
		{
			ContentLength=cl_regexp.cap(1).toInt();
			kdebug("HttpClient: Content-Length: %i bytes\n",ContentLength);			
		};
		
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
			HeaderParsed=false;
			return;
		};
	};
	// Kontynuuj odczyt jesli dane niekompletne
	// lub je¶li mamy czekaæ na connection close
	if(ContentLength>Data.size()||ContentLength<0)
		return;
	// Mamy cale dane
	kdebug("HttpClient: All Data Retreived: %i bytes\n",Data.size());
	Socket.close();
	emit finished();
};

void HttpClient::onConnectionClosed()
{
	if(HeaderParsed&&ContentLength<0)
		emit finished();
	else
		emit error();
};

void HttpClient::setHost(QString host)
{
	Host=host;
	Cookies.clear();
};

void HttpClient::get(QString path)
{
	Referer=Path;
	Path=path;
	Data.resize(0);
	PostData.resize(0);
	HeaderParsed=false;
	Socket.connectToHost(Host,80);
};

void HttpClient::post(QString path,const QByteArray& data)
{
	Referer=Path;
	Path=path;
	Data.resize(0);
	PostData.duplicate(data);
	HeaderParsed=false;
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

/********** SmsGateway **********/

SmsGateway::SmsGateway(QObject* parent)
	: QObject(parent,"SmsGateway")
{
	QObject::connect(&Http,SIGNAL(finished()),this,SLOT(httpFinished()));
	QObject::connect(&Http,SIGNAL(redirected(QString)),this,SLOT(httpRedirected(QString)));
	QObject::connect(&Http,SIGNAL(error()),this,SLOT(httpError()));
};

void SmsGateway::httpError()
{
	QMessageBox::critical((QWidget*)parent(),"SMS",tr("Network error. Provider gateway page is probably unavailable"));
	emit finished(false);
};

/********** SmsIdeaGateway **********/

SmsIdeaGateway::SmsIdeaGateway(QObject* parent)
	: SmsGateway(parent)
{
};

void SmsIdeaGateway::httpRedirected(QString link)
{
};

void SmsIdeaGateway::send(const QString& number,const QString& message, const QString& contact, const QString& signature)
{
	Number=number;
	Message=message;
	Signature= signature;
	State=SMS_LOADING_PAGE;
	Http.setHost("sms.idea.pl");
	Http.get("/");
};

bool SmsIdeaGateway::isNumberCorrect(const QString& number)
{
	return (number[0]=='5');
};

void SmsIdeaGateway::httpFinished()
{
	QDialog* p=(QDialog*)(parent()->parent());
	if(State==SMS_LOADING_PAGE)
	{
		QString Page=Http.data();
		kdebug("SMS Provider Page:\n%s\n",Page.local8Bit().data());
		QRegExp pic_regexp("rotate_token\\.aspx\\?token=([^\"]+)");
		int pic_pos=pic_regexp.search(Page);
		if(pic_pos<0)
		{
			QMessageBox::critical(p,"SMS",tr("Provider gateway page looks strange. It's probably temporary disabled\nor has beed changed too much to parse it correctly."));
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
	else if(State==SMS_LOADING_PICTURE)
	{
		kdebug("SMS Idea Picture Loaded: %i bytes\n",Http.data().size());
		SmsImageDialog* d=new SmsImageDialog(p,Http.data());
		connect(d,SIGNAL(codeEntered(const QString&)),this,SLOT(onCodeEntered(const QString&)));
		d->show();
	}
	else if(State==SMS_LOADING_RESULTS)
	{
		QString Page=Http.data();
		kdebug("SMS Provider Results Page:\n%s\n",Page.local8Bit().data());	
		if(Page.find("wyczerpany")>=0)
		{
			kdebug("You exceeded your daily limit\n");
			QMessageBox::critical(p,"SMS",tr("You exceeded your daily limit"));
			emit finished(false);
		}
		else if(Page.find("Podano b³êdne has³o")>=0)
		{
			kdebug("Text from the picture is incorrect\n");
			QMessageBox::critical(p,"SMS",tr("Text from the picture is incorrect"));
			emit finished(false);				
		}
		else if(Page.find("Odbiorca nie ma aktywnej uslugi")>=0)
		{
			kdebug("The receiver has to enable SMS STANDARD service\n");
			QMessageBox::critical(p,"SMS",tr("The receiver has to enable SMS STANDARD service"));
			emit finished(false);				
		}			
		else if(Page.find("Twój SMS zosta³ wys³any")>=0)
		{
			kdebug("SMS was sent succesfully\n");
			emit finished(true);
		}
		else
		{
			kdebug("Provider gateway results page looks strange. SMS was probably NOT sent.\n");
			QMessageBox::critical(p,"SMS",tr("Provider gateway results page looks strange. SMS was probably NOT sent."));
			emit finished(false);
		};
	}
	else
		kdebug("SMS Panic! Unknown state\n");	
};

void SmsIdeaGateway::onCodeEntered(const QString& code)
{
	if(code=="")
	{
		emit finished(false);
		return;
	};
	kdebug("SMS User entered the code\n");
	State=SMS_LOADING_RESULTS;
	QString post_data=QString("token=")+Token+"&SENDER="+Signature+"&RECIPIENT="+Number+"&SHORT_MESSAGE="+Http.encode(Message)+"&pass="+code+"&CHK_RESP=FALSE"+"&respInfo=1";
	Http.post("sendsms.aspx",post_data);
};

/********** SmsPlusGateway **********/

SmsPlusGateway::SmsPlusGateway(QObject* parent)
	: SmsGateway(parent)
{
};

void SmsPlusGateway::httpRedirected(QString link)
{
};

void SmsPlusGateway::send(const QString& number,const QString& message, const QString& contact, const QString& signature)
{
	Number=number;
	Message=message;
	State=SMS_LOADING_RESULTS;
	Http.setHost("212.2.96.57");
	QString post_data="tprefix="+Number.left(3)+"&numer="+Number.right(6)+"&odkogo="+signature+"&tekst="+Message;
	Http.post("sms/sendsms.php",post_data);
};

bool SmsPlusGateway::isNumberCorrect(const QString& number)
{
	return (number[0]=='6'&&((QChar(number[2])-'0')%2)!=0);
};

void SmsPlusGateway::httpFinished()
{
	QWidget* p=(QWidget*)(parent()->parent());
	if(State==SMS_LOADING_PAGE)
	{
		QString Page=Http.data();
		kdebug("SMS Provider Page:\n%s\n",Page.local8Bit().data());
		QRegExp code_regexp("name=\\\"kod\\\" value=\\\"(\\d+)\\\"");
		QRegExp code_regexp2("name=\\\"Kod(\\d+)\\\" value=\\\"(\\d+)\\\"");
		if(code_regexp.search(Page) < 0) {
			QMessageBox::critical(p,"SMS",tr("Provider gateway page looks strange. It's probably temporary disabled\nor has beed changed too much to parse it correctly."));
			emit finished(false);
			return;
		}
		if(code_regexp2.search(Page) < 0) {
			QMessageBox::critical(p,"SMS",tr("Provider gateway page looks strange. It's probably temporary disabled\nor has beed changed too much to parse it correctly."));
			emit finished(false);
			return;
			}
		QString code = code_regexp.cap(1);
		QString num = code_regexp2.cap(1);
		QString code2 = code_regexp2.cap(2);
		State = SMS_LOADING_RESULTS;
		QString post_data = "bookopen=&numer="+Number+"&ksiazka=ksi%B1%BFka+telefoniczna&message="+Http.encode(Message)+"&podpis="+config_file.readEntry("General","Nick")+"&kontakt=&Send=++tak-nada%E6++&Kod"+num+"="+code2+"&kod="+code;
		Http.post("sms/sendsms.asp", post_data);
	}
	else if(State==SMS_LOADING_RESULTS)
	{
		QString Page=Http.data();
		kdebug("SMS Provider Results Page:\n%s\n",Page.local8Bit().data());	
		if(Page.find("SMS zosta³ wys³any")>=0)
		{
			emit finished(true);
		}
		else
		{
			QMessageBox::critical(p,"SMS",tr("Provider gateway results page looks strange. SMS was probably NOT sent."));
			emit finished(false);
		};				
	}
	else
		kdebug("SMS Panic! Unknown state\n");	
};

/********** SmsEraGateway **********/

SmsEraGateway::SmsEraGateway(QObject* parent)
	: SmsGateway(parent)
{
};

void SmsEraGateway::send(const QString& number,const QString& message, const QString& contact, const QString& signature)
{
	Number=number;
	Message=message;
	Http.setHost("www.eraomnix.pl");
	
	QString path;
	QString post_data="login="+config_file.readEntry("SMS","EraGateway_"+config_file.readEntry("SMS", "EraGateway")+"_User")+
	    "&password="+config_file.readEntry("SMS","EraGateway_"+config_file.readEntry("SMS", "EraGateway")+"_Password")+
	    "&numbers="+number+ "&message="+message+
	    "&contact="+contact+ "&signature="+signature+
	    "&success=http://moj.serwer.pl/ok.html&failure=http://moj.serwer.pl/blad.html";
	    
	QString gateway= config_file.readEntry("SMS", "EraGateway");

	if ( gateway == "Basic")
		{
		    path= "sms/do/extern/tinker/free/send";
		    post_data.replace(post_data.find("&numbers="),9, "&number=");
		}
	else if (gateway == "Charge")
		    path= "sms/do/extern/tinker/super/send";
	else if (gateway == "Omnix")
		    path= "sms/do/extern/tinker/multi/send";
	else 
	    {
		emit finished(false);
		return;
	    }
	
	
	Http.post(path,post_data);

};

bool SmsEraGateway::isNumberCorrect(const QString& number)
{
	return (number[0]=='6'&&((QChar(number[2])-'0')%2)==0);
};

void SmsEraGateway::httpRedirected(QString link)
{
	QWidget* p=(QWidget*)(parent()->parent());
		if(link.find("ok.html")> 0)
			emit finished(true);
		else if(link.find("blad.html")> 0)
		{
			QMessageBox::critical(p,"SMS",tr("Error: ")+ SmsEraGateway::errorNumber(link.replace(link.find("http://moj.serwer.pl/blad.html?X-ERA-error="),43, "").toInt()));
			emit finished(false);
		}		
		else 
			QMessageBox::critical(p,"SMS",tr("Provider gateway results page looks strange. SMS was probably NOT sent."));
};

QString SmsEraGateway::errorNumber(int nr)
{
    switch(nr){
	case 0: return tr("No error");
		break;
	case 1: return tr("System failure");
		break;
	case 2: return tr("Unauthorised user");
		break;
	case 3: return tr("Access forbidden");
		break;
	case 5: return tr("Syntax error");
		break;
	case 7: return tr("Limit of the sms run-down");
		break;
	case 8: return tr("Wrong receiver adress");
		break;
	case 9: return tr("Message too long");
		break;
	   }
	 return tr("Unknown error");
}

void SmsEraGateway::httpFinished()
{
};

/********** SmsSender **********/

SmsSender::SmsSender(QObject* parent)
	: QObject(parent,"SmsSender")
{
};

void SmsSender::onFinished(bool success)
{
	emit finished(success);
};

void SmsSender::send(const QString& number,const QString& message, const QString& contact, const QString& signature)
{
	QString Number=number;
	if(Number.length()==12&&Number.left(3)=="+48")
		Number=Number.right(9);
	if(Number.length()!=9)
	{
		QMessageBox::critical((QWidget*)parent(), "SMS", tr("Mobile number is incorrect"));
		emit finished(false);
		return;
	};
	SmsGateway* Gateway;
	if(SmsIdeaGateway::isNumberCorrect(Number))
		Gateway=new SmsIdeaGateway(this);		
	else if(SmsPlusGateway::isNumberCorrect(Number))
		Gateway=new SmsPlusGateway(this);		
	else if(SmsEraGateway::isNumberCorrect(Number))
		Gateway=new SmsEraGateway(this);
	else		
	{
		QMessageBox::critical((QWidget*)parent(),"SMS",tr("Mobile number is incorrect"));
		emit finished(false);
		return;
	};	
	QObject::connect(Gateway, SIGNAL(finished(bool)), this, SLOT(onFinished(bool)));
	Gateway->send(Number, message, contact, signature);
};

/********** Sms **********/

Sms::Sms(const QString& altnick, QDialog* parent) : QDialog (parent, "Sms")
{
	QGridLayout * grid = new QGridLayout(this, 3, 4, 10, 3);

	body = new QMultiLineEdit(this);
	grid->addMultiCellWidget(body, 1, 1, 0, 3);
	body->setWordWrap(QMultiLineEdit::WidgetWidth);
	body->setFont(config_file.readFontEntry("Look","ChatFont"));
	QObject::connect(body, SIGNAL(textChanged()), this, SLOT(updateCounter()));

	recipient = new QLineEdit(this);
	recipient->setMinimumWidth(140);
	if(altnick!="")
		recipient->setText(userlist.byAltNick(altnick).mobile);
	QObject::connect(recipient,SIGNAL(textChanged(const QString&)),this,SLOT(updateList(const QString&)));
	grid->addWidget(recipient, 0, 1);

	QStringList strlist;
	list = new QComboBox(this);
	for (int i = 0; i < userlist.count(); i++)
		if (userlist[i].mobile.length())
		 	strlist.append(userlist[i].altnick);
	strlist.sort();
	strlist.insert(strlist.begin(), QString::null);
	list->insertStringList(strlist);
	list->setCurrentText(altnick);
	QObject::connect(list, SIGNAL(activated(const QString&)), this, SLOT(updateRecipient(const QString &)));
	grid->addWidget(list, 0, 3);

	QLabel *recilabel = new QLabel(tr("Recipient"),this);
	grid->addWidget(recilabel, 0, 0);

	smslen = new QLabel("0",this);
	grid->addWidget(smslen, 3, 0);

	b_send = new QPushButton(this);
	b_send->setText(tr("Send"));
	b_send->setMaximumWidth(200);
	grid->addWidget(b_send, 3, 3, Qt::AlignRight);
	
	l_contact= new QLabel(tr("Contact"), this);
	grid->addWidget(l_contact, 4, 0);
	e_contact= new QLineEdit(this);
	grid->addWidget(e_contact, 4, 1);
	
	l_signature= new QLabel(tr("Signature"),this);
	grid->addWidget(l_signature, 5, 0);
	e_signature= new QLineEdit(config_file.readEntry("General", "Nick"), this);
	grid->addWidget(e_signature, 5, 1);
	
	QObject::connect(b_send, SIGNAL(clicked()), this, SLOT(sendSms()));


	resize(400,250);
	setCaption(tr("Send SMS"));

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
	e_contact->setEnabled(false);
	l_contact->setEnabled(false);
	e_signature->setEnabled(false);
	l_signature->setEnabled(false);

	history.appendSms(recipient->text(), body->text());

	if(config_file.readBoolEntry("SMS","BuiltInApp"))
	{
		Sender.send(recipient->text(), body->text(), e_contact->text(), e_signature->text());
	}
	else
	{
		if(config_file.readEntry("SMS","SmsApp")=="")
		
		{
			QMessageBox::warning(this, tr("SMS error"), tr("Sms application was not specified. Visit the configuration section") );
			kdebug("SMS application NOT specified. Exit.\n");
			return;
		};
		QString SmsAppPath=config_file.readEntry("SMS","SmsApp");
		
		smsProcess = new QProcess(this);
		if(config_file.readBoolEntry("SMS","UseCustomString")&&
		(!config_file.readBoolEntry("SMS","BuiltInApp")))
		{
			QStringList args=QStringList::split(' ',config_file.readEntry("SMS","SmsString"));
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
			QMessageBox::critical(this, tr("SMS error"), tr("Could not spawn child process. Check if the program is functional") );
		QObject::connect(smsProcess, SIGNAL(processExited()), this, SLOT(smsSigHandler()));
	};
}

void Sms::smsSigHandler() {
	if (smsProcess->normalExit())
		QMessageBox::information(this, tr("SMS sent"), tr("The process exited normally. The SMS should be on its way"));
	else
		QMessageBox::warning(this, tr("SMS not sent"), tr("The process exited abnormally. The SMS may not be sent"));

	e_contact->setEnabled(true);
	l_contact->setEnabled(true);
	e_signature->setEnabled(true);
	l_signature->setEnabled(true);
	b_send->setEnabled(true);
	body->setEnabled(true);
	body->clear();
}

void Sms::updateCounter() {
	smslen->setText(QString::number(body->text().length()));
}

void Sms::onSmsSenderFinished(bool success)
{
	if(success)
	    {	
		QMessageBox::information(this, tr("SMS sent"), tr("The SMS was sent and should be on its way"));
		body->clear();
	    }
	b_send->setEnabled(true);
	body->setEnabled(true);
	e_contact->setEnabled(true);
	l_contact->setEnabled(true);
	e_signature->setEnabled(true);
	l_signature->setEnabled(true);
};

void Sms::initModule()
{

	QT_TRANSLATE_NOOP("@default", "SMS");
	QT_TRANSLATE_NOOP("@default", "SMS options");
	QT_TRANSLATE_NOOP("@default", "Use built-in SMS application");
	QT_TRANSLATE_NOOP("@default", "Custom SMS application");
	QT_TRANSLATE_NOOP("@default", "SMS custom string");
	QT_TRANSLATE_NOOP("@default", "Check this box if your sms application doesn't understand arguments: number \"message\"\nArguments should be separated with spaces. %n argument is converted to number, %m to message");
	QT_TRANSLATE_NOOP("@default", "SMS Era Gateway");
	QT_TRANSLATE_NOOP("@default", "User ID");
	QT_TRANSLATE_NOOP("@default", "Password");
	QT_TRANSLATE_NOOP("@default", "Type of gateway");


	kdebug("Sms::initModule \n");	
	
	ConfigDialog::addTab("SMS");
	ConfigDialog::addVGroupBox("SMS", "SMS", "SMS options");
	ConfigDialog::addCheckBox("SMS", "SMS options", "Use built-in SMS application", "BuiltInApp", true);
	ConfigDialog::addLineEdit("SMS", "SMS options", "Custom SMS application", "SmsApp");
	ConfigDialog::addGrid("SMS", "SMS options", "smsgrid", 2);
	ConfigDialog::addCheckBox("SMS", "smsgrid", "SMS custom string", "UseCustomString", false
	,"Check this box if your sms application doesn't understand arguments: number \"message\"\nArguments should be separated with spaces. %n argument is converted to number, %m to message");
	ConfigDialog::addLineEdit("SMS", "smsgrid", "", "SmsString", "", "", "smsstring");
	ConfigDialog::addVGroupBox("SMS", "SMS", "SMS Era Gateway");
	ConfigDialog::addComboBox("SMS", "SMS Era Gateway", "Type of gateway");

	config_file.addVariable("SMS", "EraGateway", "Omnix");
	//przepisanie starego hasla
	config_file.addVariable("SMS", "EraGateway_Omnix_User", config_file.readEntry("SMS", "EraGatewayUser"));
	config_file.addVariable("SMS", "EraGateway_Omnix_Password", config_file.readEntry("SMS", "EraGatewayPassword"));
	//

	ConfigDialog::addLineEdit2("SMS", "SMS Era Gateway", "User ID");
	ConfigDialog::addLineEdit2("SMS", "SMS Era Gateway", "Password");

	
	SmsSlots *smsslots=new SmsSlots();
	ConfigDialog::registerSlotOnCreate(smsslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnDestroy(smsslots, SLOT(onDestroyConfigDialog()));
    	ConfigDialog::connectSlot("SMS", "Use built-in SMS application", SIGNAL(toggled(bool)), smsslots, SLOT(onSmsBuildInCheckToggle(bool)));
	ConfigDialog::connectSlot("SMS", "Type of gateway", SIGNAL(activated(int)), smsslots, SLOT(onChangeEraGateway(int)));
};

void SmsSlots::onSmsBuildInCheckToggle(bool value)
{
	kdebug("SmsSlots::onSmsBuildInCheckToggle \n");

	QLineEdit *e_smsapp= ConfigDialog::getLineEdit("SMS", "Custom SMS application");
	QCheckBox *b_smscustomconf= ConfigDialog::getCheckBox("SMS", "SMS custom string");
	QLineEdit *e_smsconf= ConfigDialog::getLineEdit("SMS","","smsstring");

	((QHBox*)(e_smsapp->parent()))->setEnabled(!value);
	b_smscustomconf->setEnabled(!value);
	e_smsconf->setEnabled(b_smscustomconf->isChecked()&& !value);
};

void SmsSlots::onCreateConfigDialog()
{
	kdebug("SmsSlots::onCreateConfigDialog \n");
	
	QCheckBox *b_smsbuildin= ConfigDialog::getCheckBox("SMS", "Use built-in SMS application");
	QLineEdit *e_smsapp= ConfigDialog::getLineEdit("SMS", "Custom SMS application");
	QCheckBox *b_smscustomconf= ConfigDialog::getCheckBox("SMS", "SMS custom string");
	QLineEdit *e_smsconf= ConfigDialog::getLineEdit("SMS","","smsstring");
	
	if (b_smsbuildin->isChecked())
	{
		((QHBox*)(e_smsapp->parent()))->setEnabled(false);
		b_smscustomconf->setEnabled(false);
		e_smsconf->setEnabled(false);
	}	
			
	if (!b_smscustomconf->isChecked())
		e_smsconf->setEnabled(false);
			    
	connect(b_smscustomconf,SIGNAL(toggled(bool)),e_smsconf,SLOT(setEnabled(bool)));
	

	
	QComboBox *cb_typegateway= ConfigDialog::getComboBox("SMS","Type of gateway");
	cb_typegateway->insertItem("Basic");
	cb_typegateway->insertItem("Omnix");
	cb_typegateway->insertItem("Charge");
	cb_typegateway->setCurrentText(config_file.readEntry("SMS", "EraGateway"));
	actualEraGateway=cb_typegateway->currentText();
	
	QLineEdit *e_erauser= ConfigDialog::getLineEdit("SMS", "User ID");
	QLineEdit *e_erapassword= ConfigDialog::getLineEdit("SMS", "Password");
	e_erapassword->setEchoMode(QLineEdit::Password);
	
	e_erapassword->setText(config_file.readEntry("SMS", "EraGateway_"+config_file.readEntry("SMS", "EraGateway")+"_Password"));
	e_erauser->setText(config_file.readEntry("SMS", "EraGateway_"+config_file.readEntry("SMS", "EraGateway")+"_User"));

	
};

void SmsSlots::onDestroyConfigDialog()
{
	QComboBox *cb_typegateway= ConfigDialog::getComboBox("SMS","Type of gateway");
	config_file.writeEntry("SMS", "EraGateway",cb_typegateway->currentText());

	QLineEdit *e_erauser= ConfigDialog::getLineEdit("SMS", "User ID");
	QLineEdit *e_erapassword= ConfigDialog::getLineEdit("SMS", "Password");
	config_file.writeEntry("SMS", "EraGateway_"+config_file.readEntry("SMS", "EraGateway")+"_Password", e_erapassword->text());
	config_file.writeEntry("SMS", "EraGateway_"+config_file.readEntry("SMS", "EraGateway")+"_User", e_erauser->text());

};

void SmsSlots::onChangeEraGateway(int gateway)
{
	QLineEdit *e_erauser= ConfigDialog::getLineEdit("SMS", "User ID");
	QLineEdit *e_erapassword= ConfigDialog::getLineEdit("SMS", "Password");
	QComboBox *cb_typegateway= ConfigDialog::getComboBox("SMS","Type of gateway");
	
	config_file.writeEntry("SMS", "EraGateway_"+actualEraGateway+"_Password", e_erapassword->text());
	config_file.writeEntry("SMS", "EraGateway_"+actualEraGateway+"_User", e_erauser->text());

	e_erauser->setText(config_file.readEntry("SMS", "EraGateway_"+cb_typegateway->currentText()+"_User"));
	e_erapassword->setText(config_file.readEntry("SMS", "EraGateway_"+cb_typegateway->currentText()+"_Password"));
	actualEraGateway=cb_typegateway->text(gateway);
};
