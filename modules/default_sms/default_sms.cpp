#include "default_sms.h"

#include <qregexp.h>
#include <qmessagebox.h>

#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "modules.h"


extern "C" int sms_gateways_init()
{
	kdebugf();
	smsslots->registerGateway("idea", &SmsGatewaySlots::isValidIdea);
	smsslots->registerGateway("plus", &SmsGatewaySlots::isValidPlus);
	smsslots->registerGateway("era" , &SmsGatewaySlots::isValidEra);
	smsgatewayslots=new SmsGatewaySlots();
	return 0;
}

extern "C" void sms_gateways_close()
{
	kdebugf();
	smsslots->unregisterGateway("idea");
	smsslots->unregisterGateway("plus");
	smsslots->unregisterGateway("era");
	delete smsgatewayslots;
}


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

SmsGateway::SmsGateway(QObject* parent) : QObject(parent,"SmsGateway"){
	QObject::connect(&Http,SIGNAL(finished()),this,SLOT(httpFinished()));
	QObject::connect(&Http,SIGNAL(redirected(QString)),this,SLOT(httpRedirected(QString)));
	QObject::connect(&Http,SIGNAL(error()),this,SLOT(httpError()));
}

void SmsGateway::httpError() {
	kdebug("SmsGateway::httpError()\n");
	QMessageBox::critical((QDialog*)(parent()->parent()), "SMS",tr("Network error. Provider gateway page is probably unavailable"));
	emit finished(false);
}

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

SmsGatewaySlots::SmsGatewaySlots()
{
	QT_TRANSLATE_NOOP("@default", "SMS Era Gateway");
	QT_TRANSLATE_NOOP("@default", "User ID");
	QT_TRANSLATE_NOOP("@default", "Password");
	QT_TRANSLATE_NOOP("@default", "Type of gateway");
	ConfigDialog::addVGroupBox("SMS", "SMS", "SMS Era Gateway");
	ConfigDialog::addComboBox("SMS", "SMS Era Gateway", "Type of gateway");

	config_file.addVariable("SMS", "EraGateway", "Omnix");
	//przepisanie starego hasla
	config_file.addVariable("SMS", "EraGateway_Omnix_User", config_file.readEntry("SMS", "EraGatewayUser"));
	config_file.addVariable("SMS", "EraGateway_Omnix_Password", config_file.readEntry("SMS", "EraGatewayPassword"));
	//

	ConfigDialog::addLineEdit2("SMS", "SMS Era Gateway", "User ID");
	ConfigDialog::addLineEdit2("SMS", "SMS Era Gateway", "Password");
	ConfigDialog::registerSlotOnCreate(this, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnDestroy(this, SLOT(onDestroyConfigDialog()));
	ConfigDialog::connectSlot("SMS", "Type of gateway", SIGNAL(activated(int)), this, SLOT(onChangeEraGateway(int)));
	

}

SmsGatewaySlots::~SmsGatewaySlots()
{
	ConfigDialog::unregisterSlotOnCreate(this, SLOT(onCreateConfigDialog()));
	ConfigDialog::unregisterSlotOnDestroy(this, SLOT(onDestroyConfigDialog()));
	ConfigDialog::disconnectSlot("SMS", "Type of gateway", SIGNAL(activated(int)), this, SLOT(onChangeEraGateway(int)));
	ConfigDialog::removeControl("SMS", "Password");
	ConfigDialog::removeControl("SMS", "User ID");
	ConfigDialog::removeControl("SMS", "Type of gateway");
	ConfigDialog::removeControl("SMS", "SMS Era Gateway");
}
void SmsGatewaySlots::onChangeEraGateway(int gateway)
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

void SmsGatewaySlots::onDestroyConfigDialog()
{
	QComboBox *cb_typegateway= ConfigDialog::getComboBox("SMS","Type of gateway");
	config_file.writeEntry("SMS", "EraGateway",cb_typegateway->currentText());

	QLineEdit *e_erauser= ConfigDialog::getLineEdit("SMS", "User ID");
	QLineEdit *e_erapassword= ConfigDialog::getLineEdit("SMS", "Password");
	config_file.writeEntry("SMS", "EraGateway_"+config_file.readEntry("SMS", "EraGateway")+"_Password", e_erapassword->text());
	config_file.writeEntry("SMS", "EraGateway_"+config_file.readEntry("SMS", "EraGateway")+"_User", e_erauser->text());

};

void SmsGatewaySlots::onCreateConfigDialog()
{
	kdebug("SmsGatewaySlots::onCreateConfigDialog \n");
	
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

SmsGateway* SmsGatewaySlots::isValidIdea(QString& number, QObject* parent)
{
	if(SmsIdeaGateway::isNumberCorrect(number))
		return new SmsIdeaGateway(parent);
	else
		return NULL;
}

SmsGateway* SmsGatewaySlots::isValidPlus(QString& number, QObject* parent)
{
	if(SmsPlusGateway::isNumberCorrect(number))
		return new SmsPlusGateway(parent);
	else
		return NULL;
}

SmsGateway* SmsGatewaySlots::isValidEra(QString& number, QObject* parent)
{
	if(SmsEraGateway::isNumberCorrect(number))
		return new SmsEraGateway(parent);
	else
		return NULL;
}


SmsGatewaySlots* smsgatewayslots;
