/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
	smsslots->registerGateway("orange", &SmsGatewaySlots::isValidOrange);
	smsslots->registerGateway("plus", &SmsGatewaySlots::isValidPlus);
	smsslots->registerGateway("era" , &SmsGatewaySlots::isValidEra);
	sms_gateway_slots=new SmsGatewaySlots(NULL, "sms_gateway_slots");
	kdebugf2();
	return 0;
}

extern "C" void default_sms_close()
{
	kdebugf();
	smsslots->unregisterGateway("orange");
	smsslots->unregisterGateway("plus");
	smsslots->unregisterGateway("era");
	delete sms_gateway_slots;
	kdebugf2();
}


/********** SmsOrangeGateway **********/

SmsOrangeGateway::SmsOrangeGateway(QObject* parent, const char *name)
	: SmsGateway(parent, name), Token()
{
	modules_manager->moduleIncUsageCount("default_sms");
}

SmsOrangeGateway::~SmsOrangeGateway()
{
	modules_manager->moduleDecUsageCount("default_sms");
}

void SmsOrangeGateway::httpRedirected(QString link)
{
	kdebugmf(KDEBUG_FUNCTION_START, "link: %s\n", link.local8Bit().data());
}

void SmsOrangeGateway::send(const QString& number,const QString& message, const QString& /*contact*/, const QString& signature)
{
	kdebugf();
	Number=number;
	Message=message;
	Signature= signature;
	State=SMS_LOADING_PAGE;
	Http.setHost("sms.orange.pl");
	Http.get("/Default.aspx?id=A2B6173D-CF1A-4c38-B7A7-E3144D43D70C");
	kdebugf2();
}

bool SmsOrangeGateway::isNumberCorrect(const QString& number)
{
	return ((number[0]=='5') || (number[0]=='7' && number[1]=='8' && number[2]=='9'));
}

void SmsOrangeGateway::httpFinished()
{
	kdebugf();
	QDialog* p=(QDialog*)(parent()->parent());
	if (State==SMS_LOADING_PAGE)
	{
		QString Page=Http.data();
		kdebugm(KDEBUG_INFO, "SMS Provider Page:\n%s\n",Page.local8Bit().data());
		QRegExp pic_regexp("rotate_token\\.aspx\\?token=([^\"]+)");
		int pic_pos=pic_regexp.search(Page);
		if (pic_pos<0)
		{
			QMessageBox::critical(p,"SMS",tr("Provider gateway page looks strange. It's probably temporary disabled\nor has beed changed too much to parse it correctly."));
			emit finished(false);
			return;
		}
		QString pic_path=Page.mid(pic_pos,pic_regexp.matchedLength());
		Token=pic_regexp.cap(1);
		kdebugm(KDEBUG_INFO, "SMS Orange Token: %s\n",Token.local8Bit().data());
		kdebugm(KDEBUG_INFO, "SMS Orange Picture: %s\n",pic_path.local8Bit().data());
		State=SMS_LOADING_PICTURE;
		Http.get(pic_path);
	}
	else if (State==SMS_LOADING_PICTURE)
	{
		kdebugm(KDEBUG_INFO, "SMS Orange Picture Loaded: %i bytes\n",Http.data().size());
		SmsImageDialog* d=new SmsImageDialog(p,Http.data());
		connect(d,SIGNAL(codeEntered(const QString&)),this,SLOT(onCodeEntered(const QString&)));
		d->show();
	}
	else if (State==SMS_LOADING_RESULTS)
	{
		QString Page=Http.data();
		kdebugm(KDEBUG_INFO, "SMS Provider Results Page:\n%s\n", Page.local8Bit().data());
		if (Page.find("wyczerpany")>=0)
		{
			kdebugm(KDEBUG_INFO, "You exceeded your daily limit\n");
			QMessageBox::critical(p,"SMS",tr("You exceeded your daily limit"));
			emit finished(false);
		}
		else if (Page.find("Podano błędne hasło")>=0)
		{
			kdebugm(KDEBUG_INFO, "Text from the picture is incorrect\n");
			QMessageBox::critical(p,"SMS",tr("Text from the picture is incorrect"));
			emit finished(false);
		}
		else if (Page.find("Użytkownik nie ma aktywnej usługi")>=0)
		{
			kdebugm(KDEBUG_INFO, "The receiver has to enable SMS STANDARD service\n");
			QMessageBox::critical(p,"SMS",tr("The receiver has to enable SMS STANDARD service"));
			emit finished(false);
		}
		else if (Page.find("Twój SMS został wysłany")>=0)
		{
			kdebugm(KDEBUG_INFO, "SMS was sent succesfully\n");
			emit finished(true);
		}
		else
		{
			kdebugm(KDEBUG_INFO, "Provider gateway results page looks strange. SMS was probably NOT sent.\n");
			QMessageBox::critical(p,"SMS",tr("Provider gateway results page looks strange. SMS was probably NOT sent."));
			emit finished(false);
		}
	}
	else
		kdebugm(KDEBUG_PANIC, "SMS Panic! Unknown state\n");
	kdebugf2();
}

void SmsOrangeGateway::onCodeEntered(const QString& code)
{
	kdebugf();
	if (code.isEmpty())
	{
		emit finished(false);
		return;
	}
	kdebugm(KDEBUG_INFO, "SMS User entered the code\n");
	State=SMS_LOADING_RESULTS;
	QString post_data=QString("token=")+Token+"&SENDER="+unicodeUrl2latinUrl(Http.encode(Signature))+"&RECIPIENT="+Number+"&SHORT_MESSAGE="+unicodeUrl2latinUrl(Http.encode(Message))+"&pass="+code+"&CHK_RESP=FALSE"+"&respInfo=1";
	Http.post("sendsms.aspx",post_data);
	kdebugf2();
}

/********** SmsPlusGateway **********/

SmsPlusGateway::SmsPlusGateway(QObject* parent, const char *name)
	: SmsGateway(parent, name)
{
	modules_manager->moduleIncUsageCount("default_sms");
}

SmsPlusGateway::~SmsPlusGateway()
{
	modules_manager->moduleDecUsageCount("default_sms");
}

void SmsPlusGateway::httpRedirected(QString link)
{
	kdebugmf(KDEBUG_FUNCTION_START, "link: %s\n", link.local8Bit().data());
}

void SmsPlusGateway::send(const QString& number, const QString& message, const QString& /*contact*/, const QString& signature)
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
	return 
		((number[0]=='6' && ((QChar(number[2])-'0')%2)!=0) ||
		(number[0]=='7' && number[1]=='8' && (number[2]=='1' || number[2]=='3')));
}

void SmsPlusGateway::httpFinished()
{
	kdebugf();
	QWidget* p=(QWidget*)(parent()->parent());
	if (State==SMS_LOADING_PAGE)
	{
		QString Page=Http.data();
		kdebugm(KDEBUG_INFO, "SMS Provider Page:\n%s\n",Page.local8Bit().data());
		QRegExp code_regexp("name=\\\"kod\\\" value=\\\"(\\d+)\\\"");
		QRegExp code_regexp2("name=\\\"Kod(\\d+)\\\" value=\\\"(\\d+)\\\"");
		if (code_regexp.search(Page) < 0)
		{
			QMessageBox::critical(p,"SMS",tr("Provider gateway page looks strange. It's probably temporary disabled\nor has beed changed too much to parse it correctly."));
			emit finished(false);
			return;
		}
		if (code_regexp2.search(Page) < 0)
		{
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
	else if (State==SMS_LOADING_RESULTS)
	{
		QString Page=Http.data();
		kdebugm(KDEBUG_INFO, "SMS Provider Results Page:\n%s\n",Page.local8Bit().data());
		if (Page.find("Z powodu przekroczenia limit�w bramki")>=0)
		{
			kdebugm(KDEBUG_INFO, "Limit exceeded\n");
			QMessageBox::critical(p,"SMS",tr("Limits have been exceeded, try again later."));
			emit finished(false);
		}
		else if (Page.find("SMS zosta� wys�any")>=0)
			emit finished(true);
		else
		{
			QMessageBox::critical(p,"SMS",tr("Provider gateway results page looks strange. SMS was probably NOT sent."));
			emit finished(false);
		}
	}
	else
		kdebugm(KDEBUG_PANIC, "SMS Panic! Unknown state\n");
	kdebugf2();
}

/********** SmsEraGateway **********/

SmsEraGateway::SmsEraGateway(QObject* parent, const char *name)
	: SmsGateway(parent, name)
{
	modules_manager->moduleIncUsageCount("default_sms");
}

SmsEraGateway::~SmsEraGateway()
{
	modules_manager->moduleDecUsageCount("default_sms");
}

void SmsEraGateway::send(const QString& number, const QString& message, const QString& /*contact*/, const QString& signature)
{
	kdebugf();
	Number=number;
	Message=message;
	Http.setHost("www.eraomnix.pl");

	QString path;
	QString gateway = config_file.readEntry("SMS", "EraGateway");
	QString post_data = "login=" + config_file.readEntry("SMS","EraGateway_" + gateway + "_User") +
	    "&password=" + config_file.readEntry("SMS","EraGateway_" + gateway + "_Password") +
	    "&number=48" + number + "&message=" + unicode2std(signature) + ":" + unicode2std(message) + "&mms=no" +
	    "&success=OK&failure=ERROR";

	if (gateway == "Sponsored")
	{
		path= "msg/api/do/tinker/sponsored";
	}
	else if (gateway == "OmnixMultimedia")
	{
		path= "msg/api/do/tinker/omnix";
		post_data.replace("&number=48", "&numbers=");
	}
	else
	{
		emit finished(false);
		return;
	}

	Http.post(path, post_data);
	kdebugf2();
}

bool SmsEraGateway::isNumberCorrect(const QString& number)
{
	return
		((number[0]=='6'&&((QChar(number[2])-'0')%2)==0) ||
		(number[0]=='7' && number[1]=='8' && number[2]=='7') ||
		(number[0]=='8' && number[1]=='8' && number[2]=='8') ||
		(number[0]=='8' && number[1]=='8' && number[2]=='9') ||
		(number[0]=='8' && number[1]=='8' && number[2]=='0') ||
		(number[0]=='8' && number[1]=='8' && number[2]=='6'));
}

void SmsEraGateway::httpRedirected(QString link)
{
	kdebugmf(KDEBUG_FUNCTION_START, "link: %s\n", link.local8Bit().data());
	QWidget* p=(QWidget*)(parent()->parent());
	if (link.find("OK")> 0)
	{
		if (config_file.readEntry("SMS", "EraGateway") == "Sponsored")
			QMessageBox::information(p, "SMS", tr("Number of SMS' left on Sponsored Era Gateway: ")+ link.remove("http://www.eraomnix.pl/msg/api/do/tinker/OK?X-ERA-error=0&X-ERA-counter="), QMessageBox::Ok);
		emit finished(true);
	}
	else if (link.find("ERROR")> 0)
	{
		link.remove("http://www.eraomnix.pl/msg/api/do/tinker/ERROR?X-ERA-error=");
		link.remove(link.find("&X-ERA-counter="), 17);
		QMessageBox::critical(p, "SMS", tr("Error: ")+ SmsEraGateway::errorNumber(link.toInt()));
		emit finished(false);
	}
	else
		QMessageBox::critical(p, "SMS", tr("Provider gateway results page looks strange. SMS was probably NOT sent."));
	kdebugf2();
}

QString SmsEraGateway::errorNumber(int nr)
{
	switch(nr)
	{
		case 0: return tr("No error");
		case 1: return tr("System failure");
		case 2: return tr("Unauthorised user");
		case 3: return tr("Access forbidden");
		case 5: return tr("Syntax error");
		case 7: return tr("Limit of the sms run-down");
		case 8: return tr("Wrong receiver address");
		case 9: return tr("Message too long");
		case 10: return tr("You don't have enough tokens");
	}
	return tr("Unknown error (%1)").arg(nr);
}

void SmsEraGateway::httpFinished()
{
}

SmsGatewaySlots::SmsGatewaySlots(QObject *parent, const char *name) :
	QObject(parent, name), era_types(), era_values(), actualEraGateway()
{
	kdebugf();
	ConfigDialog::addVGroupBox("SMS", "sms-beginner",
			QT_TRANSLATE_NOOP("@default", "SMS Era Gateway"));

	era_types=toStringList(tr("Sponsored"), tr("OmnixMultimedia"));
	era_values=toStringList("Sponsored", "OmnixMultimedia");
	ConfigDialog::addComboBox("SMS", "SMS Era Gateway",
			QT_TRANSLATE_NOOP("@default", "Type of gateway"), "EraGateway", era_types, era_values);

	config_file.addVariable("SMS", "EraGateway", "Sponsored");
	//przepisanie starego hasla
	config_file.addVariable("SMS", "EraGateway_Omnix_User", config_file.readEntry("SMS", "EraGatewayUser"));
	config_file.addVariable("SMS", "EraGateway_Omnix_Password", config_file.readEntry("SMS", "EraGatewayPassword"));
	config_file.addVariable("SMS", "EraGateway_OmnixMultimedia_User", config_file.readEntry("SMS", "EraGateway_Omnix_User"));
	config_file.addVariable("SMS", "EraGateway_OmnixMultimedia_Password", config_file.readEntry("SMS", "EraGateway_Omnix_Password"));
	config_file.addVariable("SMS", "EraGateway_Sponsored_User", config_file.readEntry("SMS", "EraGateway_Basic_User"));
	config_file.addVariable("SMS", "EraGateway_Sponsored_Password", config_file.readEntry("SMS", "EraGateway_Basic_Password"));
	//

	ConfigDialog::addLineEdit2("SMS", "SMS Era Gateway",
			QT_TRANSLATE_NOOP("@default", "User ID (48xxxxxxxxx)"));
	ConfigDialog::addLineEdit2("SMS", "SMS Era Gateway",
			QT_TRANSLATE_NOOP("@default", "Password"));

	ConfigDialog::registerSlotOnCreateTab("SMS", this, SLOT(onCreateTabSMS()));
	ConfigDialog::registerSlotOnCloseTab("SMS", this, SLOT(onCloseTabSMS()));
	ConfigDialog::registerSlotOnApplyTab("SMS", this, SLOT(onApplyTabSMS()));
	ConfigDialog::connectSlot("SMS", "Type of gateway", SIGNAL(activated(int)), this, SLOT(onChangeEraGateway(int)));
	kdebugf2();
}

SmsGatewaySlots::~SmsGatewaySlots()
{
	kdebugf();
	ConfigDialog::unregisterSlotOnCreateTab("SMS", this, SLOT(onCreateTabSMS()));
	ConfigDialog::unregisterSlotOnCloseTab("SMS", this, SLOT(onCloseTabSMS()));
	ConfigDialog::unregisterSlotOnApplyTab("SMS", this, SLOT(onApplyTabSMS()));

	ConfigDialog::disconnectSlot("SMS", "Type of gateway", SIGNAL(activated(int)), this, SLOT(onChangeEraGateway(int)));
	ConfigDialog::removeControl("SMS", "Password");
	ConfigDialog::removeControl("SMS", "User ID (48xxxxxxxxx)");
	ConfigDialog::removeControl("SMS", "Type of gateway");
	ConfigDialog::removeControl("SMS", "SMS Era Gateway");
	kdebugf2();
}

void SmsGatewaySlots::onChangeEraGateway(int gateway)
{
	kdebugf();
	QLineEdit *e_erauser= ConfigDialog::getLineEdit("SMS", "User ID (48xxxxxxxxx)");
	QLineEdit *e_erapassword= ConfigDialog::getLineEdit("SMS", "Password");

	config_file.writeEntry("SMS", "EraGateway_"+ actualEraGateway+ "_Password", e_erapassword->text());
	config_file.writeEntry("SMS", "EraGateway_"+ actualEraGateway+ "_User", e_erauser->text());

	e_erauser->setText(config_file.readEntry("SMS", "EraGateway_"+ era_values[gateway]+ "_User", "48"));
	e_erapassword->setText(config_file.readEntry("SMS", "EraGateway_"+ era_values[gateway]+ "_Password"));
	actualEraGateway=era_values[gateway];
	kdebugf2();
}

void SmsGatewaySlots::onApplyTabSMS()
{
	kdebugf();

	QLineEdit *e_erauser= ConfigDialog::getLineEdit("SMS", "User ID (48xxxxxxxxx)");
	QLineEdit *e_erapassword= ConfigDialog::getLineEdit("SMS", "Password");
	QString gateway= config_file.readEntry("SMS", "EraGateway");

	config_file.writeEntry("SMS", "EraGateway_"+ gateway+ "_Password", e_erapassword->text());
	config_file.writeEntry("SMS", "EraGateway_"+ gateway+ "_User", e_erauser->text());
	kdebugf2();
}

static bool activated;

void SmsGatewaySlots::onCloseTabSMS()
{
	kdebugf();
	if (activated)
		modules_manager->moduleDecUsageCount("default_sms");
	activated=false;
}

void SmsGatewaySlots::onCreateTabSMS()
{
	kdebugf();

	actualEraGateway=config_file.readEntry("SMS", "EraGateway");

	QLineEdit *e_erauser= ConfigDialog::getLineEdit("SMS", "User ID (48xxxxxxxxx)");
	QLineEdit *e_erapassword= ConfigDialog::getLineEdit("SMS", "Password");
	e_erapassword->setEchoMode(QLineEdit::Password);

	e_erapassword->setText(config_file.readEntry("SMS", "EraGateway_"+ actualEraGateway+ "_Password"));
	e_erauser->setText(config_file.readEntry("SMS", "EraGateway_"+ actualEraGateway+ "_User", "48"));

	modules_manager->moduleIncUsageCount("default_sms");
	activated=true;
	kdebugf2();
}

SmsGateway* SmsGatewaySlots::isValidOrange(const QString& number, QObject* parent)
{
	if(SmsOrangeGateway::isNumberCorrect(number))
		return new SmsOrangeGateway(parent, "sms_orange_gateway");
	else
		return NULL;
}

SmsGateway* SmsGatewaySlots::isValidPlus(const QString& number, QObject* parent)
{
	if(SmsPlusGateway::isNumberCorrect(number))
		return new SmsPlusGateway(parent, "sms_plus_gateway");
	else
		return NULL;
}

SmsGateway* SmsGatewaySlots::isValidEra(const QString& number, QObject* parent)
{
	if(SmsEraGateway::isNumberCorrect(number))
		return new SmsEraGateway(parent, "sms_era_gateway");
	else
		return NULL;
}

SmsGatewaySlots* sms_gateway_slots;
