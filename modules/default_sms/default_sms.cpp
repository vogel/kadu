#include "default_sms.h"

#include <qregexp.h>
#include <qmessagebox.h>

#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "modules.h"

extern "C" int default_sms_init()
{
	kdebugf();
	smsslots->registerGateway("idea", &SmsGatewaySlots::isValidIdea);
	smsslots->registerGateway("plus", &SmsGatewaySlots::isValidPlus);
	smsslots->registerGateway("era" , &SmsGatewaySlots::isValidEra);
	smsgatewayslots=new SmsGatewaySlots();
	kdebugf2();
	return 0;
}

extern "C" void default_sms_close()
{
	kdebugf();
	smsslots->unregisterGateway("idea");
	smsslots->unregisterGateway("plus");
	smsslots->unregisterGateway("era");
	delete smsgatewayslots;
	kdebugf2();
}


/********** SmsIdeaGateway **********/

SmsIdeaGateway::SmsIdeaGateway(QObject* parent)
	: SmsGateway(parent)
{
	modules_manager->moduleIncUsageCount("default_sms");
}

SmsIdeaGateway::~SmsIdeaGateway()
{
	modules_manager->moduleDecUsageCount("default_sms");
}

void SmsIdeaGateway::httpRedirected(QString link)
{
	kdebugf();
}

void SmsIdeaGateway::send(const QString& number,const QString& message, const QString& contact, const QString& signature)
{
	kdebugf();
	Number=number;
	Message=message;
	Signature= signature;
	State=SMS_LOADING_PAGE;
	Http.setHost("sms.idea.pl");
	Http.get("/");
	kdebugf2();
}

bool SmsIdeaGateway::isNumberCorrect(const QString& number)
{
	return (number[0]=='5');
}

void SmsIdeaGateway::httpFinished()
{
	kdebugf();
	QDialog* p=(QDialog*)(parent()->parent());
	if(State==SMS_LOADING_PAGE)
	{
		QString Page=Http.data();
		kdebugm(KDEBUG_INFO, "SMS Provider Page:\n%s\n",Page.local8Bit().data());
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
		kdebugm(KDEBUG_INFO, "SMS Idea Token: %s\n",Token.local8Bit().data());
		kdebugm(KDEBUG_INFO, "SMS Idea Picture: %s\n",pic_path.local8Bit().data());
		State=SMS_LOADING_PICTURE;
		Http.get(pic_path);
	}
	else if(State==SMS_LOADING_PICTURE)
	{
		kdebugm(KDEBUG_INFO, "SMS Idea Picture Loaded: %i bytes\n",Http.data().size());
		SmsImageDialog* d=new SmsImageDialog(p,Http.data());
		connect(d,SIGNAL(codeEntered(const QString&)),this,SLOT(onCodeEntered(const QString&)));
		d->show();
	}
	else if(State==SMS_LOADING_RESULTS)
	{
		QString Page=Http.data();
		kdebugm(KDEBUG_INFO, "SMS Provider Results Page:\n%s\n",Page.local8Bit().data());	
		if(Page.find("wyczerpany")>=0)
		{
			kdebugm(KDEBUG_INFO, "You exceeded your daily limit\n");
			QMessageBox::critical(p,"SMS",tr("You exceeded your daily limit"));
			emit finished(false);
		}
		else if(Page.find("Podano b³êdne has³o")>=0)
		{
			kdebugm(KDEBUG_INFO, "Text from the picture is incorrect\n");
			QMessageBox::critical(p,"SMS",tr("Text from the picture is incorrect"));
			emit finished(false);				
		}
		else if(Page.find("Odbiorca nie ma aktywnej uslugi")>=0)
		{
			kdebugm(KDEBUG_INFO, "The receiver has to enable SMS STANDARD service\n");
			QMessageBox::critical(p,"SMS",tr("The receiver has to enable SMS STANDARD service"));
			emit finished(false);				
		}			
		else if(Page.find("Twój SMS zosta³ wys³any")>=0)
		{
			kdebugm(KDEBUG_INFO, "SMS was sent succesfully\n");
			emit finished(true);
		}
		else
		{
			kdebugm(KDEBUG_INFO, "Provider gateway results page looks strange. SMS was probably NOT sent.\n");
			QMessageBox::critical(p,"SMS",tr("Provider gateway results page looks strange. SMS was probably NOT sent."));
			emit finished(false);
		};
	}
	else
		kdebugm(KDEBUG_PANIC, "SMS Panic! Unknown state\n");
	kdebugf2();
}

void SmsIdeaGateway::onCodeEntered(const QString& code)
{
	kdebugf();
	if(code=="")
	{
		emit finished(false);
		return;
	}
	kdebugm(KDEBUG_INFO, "SMS User entered the code\n");
	State=SMS_LOADING_RESULTS;
	QString post_data=QString("token=")+Token+"&SENDER="+Signature+"&RECIPIENT="+Number+"&SHORT_MESSAGE="+Http.encode(Message)+"&pass="+code+"&CHK_RESP=FALSE"+"&respInfo=1";
	Http.post("sendsms.aspx",post_data);
	kdebugf2();
}

/********** SmsPlusGateway **********/

SmsPlusGateway::SmsPlusGateway(QObject* parent)
	: SmsGateway(parent)
{
	modules_manager->moduleIncUsageCount("default_sms");
}

SmsPlusGateway::~SmsPlusGateway()
{
	modules_manager->moduleDecUsageCount("default_sms");
}

void SmsPlusGateway::httpRedirected(QString link)
{
}

void SmsPlusGateway::send(const QString& number,const QString& message, const QString& contact, const QString& signature)
{
	kdebugf();
	Number=number;
	Message=message;
	State=SMS_LOADING_RESULTS;
	Http.setHost("212.2.96.57");
	QString post_data="tprefix="+Number.left(3)+"&numer="+Number.right(6)+"&odkogo="+signature+"&tekst="+Message;
	Http.post("sms/sendsms.php",post_data);
	kdebugf2();
}

bool SmsPlusGateway::isNumberCorrect(const QString& number)
{
	return (number[0]=='6'&&((QChar(number[2])-'0')%2)!=0);
}

void SmsPlusGateway::httpFinished()
{
	kdebugf();
	QWidget* p=(QWidget*)(parent()->parent());
	if(State==SMS_LOADING_PAGE)
	{
		QString Page=Http.data();
		kdebugm(KDEBUG_INFO, "SMS Provider Page:\n%s\n",Page.local8Bit().data());
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
		kdebugm(KDEBUG_INFO, "SMS Provider Results Page:\n%s\n",Page.local8Bit().data());	
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
		kdebugm(KDEBUG_PANIC, "SMS Panic! Unknown state\n");	
	kdebugf2();
}

/********** SmsEraGateway **********/

SmsEraGateway::SmsEraGateway(QObject* parent)
	: SmsGateway(parent)
{
	modules_manager->moduleIncUsageCount("default_sms");
}

SmsEraGateway::~SmsEraGateway()
{
	modules_manager->moduleDecUsageCount("default_sms");
}

void SmsEraGateway::send(const QString& number,const QString& message, const QString& contact, const QString& signature)
{
	kdebugf();
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
	kdebugf2();
}

bool SmsEraGateway::isNumberCorrect(const QString& number)
{
	return ((number[0]=='6'&&((QChar(number[2])-'0')%2)==0) || (number[0]=='8' && number[1]=='8' && number[2]=='8'));
}

void SmsEraGateway::httpRedirected(QString link)
{
	kdebugf();
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
	kdebugf2();
}

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
}

SmsGatewaySlots::SmsGatewaySlots()
{
	kdebugf();
	ConfigDialog::addVGroupBox("SMS", "SMS",
			QT_TRANSLATE_NOOP("@default", "SMS Era Gateway"));
	ConfigDialog::addComboBox("SMS", "SMS Era Gateway",
			QT_TRANSLATE_NOOP("@default", "Type of gateway"));

	config_file.addVariable("SMS", "EraGateway", "Omnix");
	//przepisanie starego hasla
	config_file.addVariable("SMS", "EraGateway_Omnix_User", config_file.readEntry("SMS", "EraGatewayUser"));
	config_file.addVariable("SMS", "EraGateway_Omnix_Password", config_file.readEntry("SMS", "EraGatewayPassword"));
	//

	ConfigDialog::addLineEdit2("SMS", "SMS Era Gateway",
			QT_TRANSLATE_NOOP("@default", "User ID"));
	ConfigDialog::addLineEdit2("SMS", "SMS Era Gateway",
			QT_TRANSLATE_NOOP("@default", "Password"));

	ConfigDialog::registerSlotOnCreate(this, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnClose(this, SLOT(onCloseConfigDialog()));
	ConfigDialog::registerSlotOnApply(this, SLOT(onApplyConfigDialog()));
	ConfigDialog::connectSlot("SMS", "Type of gateway", SIGNAL(activated(int)), this, SLOT(onChangeEraGateway(int)));
	kdebugf2();
}

SmsGatewaySlots::~SmsGatewaySlots()
{
	kdebugf();
	ConfigDialog::unregisterSlotOnCreate(this, SLOT(onCreateConfigDialog()));
	ConfigDialog::unregisterSlotOnClose(this, SLOT(onCloseConfigDialog()));
	ConfigDialog::unregisterSlotOnApply(this, SLOT(onApplyConfigDialog()));
	
	ConfigDialog::disconnectSlot("SMS", "Type of gateway", SIGNAL(activated(int)), this, SLOT(onChangeEraGateway(int)));
	ConfigDialog::removeControl("SMS", "Password");
	ConfigDialog::removeControl("SMS", "User ID");
	ConfigDialog::removeControl("SMS", "Type of gateway");
	ConfigDialog::removeControl("SMS", "SMS Era Gateway");
	kdebugf2();
}

void SmsGatewaySlots::onChangeEraGateway(int gateway)
{
	kdebugf();
	QLineEdit *e_erauser= ConfigDialog::getLineEdit("SMS", "User ID");
	QLineEdit *e_erapassword= ConfigDialog::getLineEdit("SMS", "Password");
	QComboBox *cb_typegateway= ConfigDialog::getComboBox("SMS","Type of gateway");
	
	config_file.writeEntry("SMS", "EraGateway_"+actualEraGateway+"_Password", e_erapassword->text());
	config_file.writeEntry("SMS", "EraGateway_"+actualEraGateway+"_User", e_erauser->text());

	e_erauser->setText(config_file.readEntry("SMS", "EraGateway_"+cb_typegateway->currentText()+"_User"));
	e_erapassword->setText(config_file.readEntry("SMS", "EraGateway_"+cb_typegateway->currentText()+"_Password"));
	actualEraGateway=cb_typegateway->text(gateway);
	kdebugf2();
}

void SmsGatewaySlots::onApplyConfigDialog()
{
	kdebugf();
	
	QComboBox *cb_typegateway= ConfigDialog::getComboBox("SMS","Type of gateway");
	config_file.writeEntry("SMS", "EraGateway",cb_typegateway->currentText());

	QLineEdit *e_erauser= ConfigDialog::getLineEdit("SMS", "User ID");
	QLineEdit *e_erapassword= ConfigDialog::getLineEdit("SMS", "Password");
	config_file.writeEntry("SMS", "EraGateway_"+config_file.readEntry("SMS", "EraGateway")+"_Password", e_erapassword->text());
	config_file.writeEntry("SMS", "EraGateway_"+config_file.readEntry("SMS", "EraGateway")+"_User", e_erauser->text());
	kdebugf2();
}

void SmsGatewaySlots::onCloseConfigDialog()
{
	kdebugf();
	modules_manager->moduleDecUsageCount("default_sms");
}

void SmsGatewaySlots::onCreateConfigDialog()
{
	kdebugf();
	
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

	modules_manager->moduleIncUsageCount("default_sms");
	kdebugf2();
}

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
