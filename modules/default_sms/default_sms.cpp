/*
 * %kadu copyright begin%
 * Copyright 2006, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2005 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QRegExp>
#include <QtGui/QMessageBox>
#include <QtCore/QUrl>

#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-line-edit.h"
#include "configuration/configuration-file.h"
#include "debug.h"
#include "misc/misc.h"
#include "misc/path-conversion.h"
#include "modules.h"

#include "default_sms.h"

/**
 * @ingroup default_sms
 * @{
 */
extern "C" KADU_EXPORT int default_sms_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	SmsGatewayManager::instance()->registerGateway(new SmsOrangeGateway());
	SmsGatewayManager::instance()->registerGateway(new SmsPlusGateway());
	SmsGatewayManager::instance()->registerGateway(new SmsEraGateway());

	defaultSmsConfigurationUiHandler = new DefaultSmsConfigurationUiHandler();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/default_sms.ui"));
	MainConfigurationWindow::registerUiHandler(defaultSmsConfigurationUiHandler);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void default_sms_close()
{
	kdebugf();

	SmsGatewayManager::instance()->unregisterGateway("orange");
	SmsGatewayManager::instance()->unregisterGateway("plus");
	SmsGatewayManager::instance()->unregisterGateway("era");

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/default_sms.ui"));
	MainConfigurationWindow::unregisterUiHandler(defaultSmsConfigurationUiHandler);
	delete defaultSmsConfigurationUiHandler;
	defaultSmsConfigurationUiHandler = 0;

	kdebugf2();
}


/********** SmsOrangeGateway **********/

SmsOrangeGateway::SmsOrangeGateway()
	: SmsGateway(), Token()
{
	ModulesManager::instance()->moduleIncUsageCount("default_sms");
}

SmsOrangeGateway::~SmsOrangeGateway()
{
	ModulesManager::instance()->moduleDecUsageCount("default_sms");
}

void SmsOrangeGateway::httpRedirected(QString link)
{
	Q_UNUSED(link)

	kdebugmf(KDEBUG_FUNCTION_START, "link: %s\n", qPrintable(link));
}

void SmsOrangeGateway::send(const QString& number,const QString& message, const QString& /*contact*/, const QString& signature)
{
	kdebugf();

	Number = number;
	Message = unicode2std(message);
	Signature = unicode2std(signature);
	State = SMS_LOADING_PAGE;
	Http.setHost("sms.orange.pl");
	Http.get("/Default.aspx?id=A2B6173D-CF1A-4c38-B7A7-E3144D43D70C");

	kdebugf2();
}

void SmsOrangeGateway::httpFinished()
{
	kdebugf();

	if (State==SMS_LOADING_PAGE)
	{
		QString Page=Http.data();
		kdebugm(KDEBUG_INFO, "SMS Provider Page:\n%s\n", qPrintable(Page));
		QRegExp pic_regexp("rotate_token\\.aspx\\?token=([^\"]+)");
		int pic_pos=pic_regexp.indexIn(Page);
		if (pic_pos<0)
		{
			QMessageBox::critical(NULL,"SMS",tr("Provider gateway page looks strange. It's probably temporary disabled\nor has beed changed too much to parse it correctly."));
			emit finished(false);
			return;
		}
		QString pic_path=Page.mid(pic_pos,pic_regexp.matchedLength());
		Token=pic_regexp.cap(1);
		kdebugm(KDEBUG_INFO, "SMS Orange Token: %s\n", qPrintable(Token));
		kdebugm(KDEBUG_INFO, "SMS Orange Picture: %s\n", qPrintable(pic_path));
		State=SMS_LOADING_PICTURE;
		Http.get(pic_path);
	}
	else if (State==SMS_LOADING_PICTURE)
	{
		//TODO 0.6.6 Orange token does not appear in SmsImageDialog
		kdebugm(KDEBUG_INFO, "SMS Orange Picture Loaded: %i bytes\n",Http.data().size());
		SmsImageDialog* d=new SmsImageDialog(NULL,Http.data());
		connect(d,SIGNAL(codeEntered(const QString&)),this,SLOT(onCodeEntered(const QString&)));
		d->show();
	}
	else if (State==SMS_LOADING_RESULTS)
	{
		QString Page=Http.data();
		kdebugm(KDEBUG_INFO, "SMS Provider Results Page:\n%s\n", qPrintable(Page));
		if (Page.indexOf("wyczerpany")>=0)
		{
			kdebugm(KDEBUG_INFO, "You exceeded your daily limit\n");
			QMessageBox::critical(NULL,"SMS",tr("You exceeded your daily limit"));
			emit finished(false);
		}
		else if (Page.indexOf("Podano błędne hasło")>=0)
		{
			kdebugm(KDEBUG_INFO, "Text from the picture is incorrect\n");
			QMessageBox::critical(NULL,"SMS",tr("Text from the picture is incorrect"));
			emit finished(false);
		}
		else if (Page.indexOf("Użytkownik nie ma aktywnej usługi")>=0)
		{
			kdebugm(KDEBUG_INFO, "The receiver has to enable SMS STANDARD service\n");
			QMessageBox::critical(NULL,"SMS",tr("The receiver has to enable SMS STANDARD service"));
			emit finished(false);
		}
		else if (Page.indexOf("Twój SMS został wysłany")>=0)
		{
			kdebugm(KDEBUG_INFO, "SMS was sent succesfully\n");
			emit finished(true);
		}
		else if (Page.indexOf("Wiadomość została pomyślnie wysłana")>=0)
		{
			kdebugm(KDEBUG_INFO, "SMS was sent succesfully\n");
			emit finished(true);
		}
		else if (Page.indexOf("Wiadomość została pomyślnie wysłana")>=0)
		{
			kdebugm(KDEBUG_INFO, "SMS was sent succesfully\n");
			emit finished(true);
		}
		else if (Page.find("Wiadomość została pomyślnie wysłana")>=0)
		{
			kdebugm(KDEBUG_INFO, "SMS was sent succesfully\n");
			emit finished(true);
		}
		else
		{
			kdebugm(KDEBUG_INFO, "Provider gateway results page looks strange. SMS was probably NOT sent.\n");
			QMessageBox::critical(NULL,"SMS",tr("Provider gateway results page looks strange. SMS was probably NOT sent."));
			emit finished(false);
		}
	}
	else
	{
		kdebugm(KDEBUG_PANIC, "SMS Panic! Unknown state\n");
	}

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
	State = SMS_LOADING_RESULTS;
	QString post_data=QString("token=")+Token+"&SENDER="+unicodeUrl2latinUrl(QUrl::toPercentEncoding(Signature))+"&RECIPIENT="+Number+"&SHORT_MESSAGE="+unicodeUrl2latinUrl(QUrl::toPercentEncoding(Message))+"&pass="+code+"&CHK_RESP=FALSE"+"&respInfo=1";
	Http.post("sendsms.aspx",post_data);
	kdebugf2();
}

/********** SmsPlusGateway **********/

SmsPlusGateway::SmsPlusGateway()
	: SmsGateway()
{
	ModulesManager::instance()->moduleIncUsageCount("default_sms");
}

SmsPlusGateway::~SmsPlusGateway()
{
	ModulesManager::instance()->moduleDecUsageCount("default_sms");
}

void SmsPlusGateway::httpRedirected(QString link)
{
	Q_UNUSED(link)

	kdebugmf(KDEBUG_FUNCTION_START, "link: %s\n", qPrintable(link));
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

void SmsPlusGateway::httpFinished()
{
	kdebugf();

	if (State==SMS_LOADING_PAGE)
	{
		QString Page=Http.data();
		kdebugm(KDEBUG_INFO, "SMS Provider Page:\n%s\n", qPrintable(Page));
		QRegExp code_regexp("name=\\\"kod\\\" value=\\\"(\\d+)\\\"");
		QRegExp code_regexp2("name=\\\"Kod(\\d+)\\\" value=\\\"(\\d+)\\\"");
		if (code_regexp.indexIn(Page) < 0)
		{
			QMessageBox::critical(0,"SMS",tr("Provider gateway page looks strange. It's probably temporary disabled\nor has beed changed too much to parse it correctly."));
			emit finished(false);
			return;
		}
		if (code_regexp2.indexIn(Page) < 0)
		{
			QMessageBox::critical(0,"SMS",tr("Provider gateway page looks strange. It's probably temporary disabled\nor has beed changed too much to parse it correctly."));
			emit finished(false);
			return;
		}
		QString code = code_regexp.cap(1);
		QString num = code_regexp2.cap(1);
		QString code2 = code_regexp2.cap(2);
		State = SMS_LOADING_RESULTS;
		QString post_data = "bookopen=&numer="+Number+"&ksiazka=ksi%B1%BFka+telefoniczna&message="+QUrl::toPercentEncoding(Message)+"&podpis="+config_file.readEntry("General","Nick")+"&kontakt=&Send=++tak-nada%E6++&Kod"+num+"="+code2+"&kod="+code;
		Http.post("sms/sendsms.asp", post_data);
	}
	else if (State==SMS_LOADING_RESULTS)
	{
		QString Page=Http.data();
		kdebugm(KDEBUG_INFO, "SMS Provider Results Page:\n%s\n", qPrintable(Page));
		if (Page.indexOf("Z powodu przekroczenia limitów bramki")>=0)
		{
			kdebugm(KDEBUG_INFO, "Limit exceeded\n");
			QMessageBox::critical(0,"SMS",tr("Limits have been exceeded, try again later."));
			emit finished(false);
		}
		else if (Page.indexOf("SMS został wysłany")>=0)
			emit finished(true);
		else if (Page.indexOf("wiadomość została wysłana")>=0)
			emit finished(true);
		else
		{
			QMessageBox::critical(0,"SMS",tr("Provider gateway results page looks strange. SMS was probably NOT sent."));
			emit finished(false);
		}
	}
	else
	{
		kdebugm(KDEBUG_PANIC, "SMS Panic! Unknown state\n");
	}

	kdebugf2();
}

/********** SmsEraGateway **********/

SmsEraGateway::SmsEraGateway()
	: SmsGateway()
{
	createDefaultConfiguration();

	ModulesManager::instance()->moduleIncUsageCount("default_sms");
}

SmsEraGateway::~SmsEraGateway()
{
	ModulesManager::instance()->moduleDecUsageCount("default_sms");
}

void SmsEraGateway::send(const QString& number, const QString& message, const QString& /*contact*/, const QString& signature)
{
	kdebugf();

	Number = number;
	Message = message;

	QString path;
	QString gateway = config_file.readEntry("SMS", "EraGateway");
	QString get_data = "?failure=localhost&success=localhost&login=" + config_file.readEntry("SMS", "EraGateway_" + gateway + "_User") +
		"&password=" + config_file.readEntry("SMS","EraGateway_" + gateway + "_Password") +
		"&number=48" + number + "&message=" + unicode2std(signature) + ":" + unicode2std(message) + "&mms=no";

	if (gateway == "Sponsored")
	{
		path= "msg/api/do/tinker/sponsored";
	}
	else if (gateway == "OmnixMultimedia")
	{
		path= "msg/api/do/tinker/omnix";
		get_data.replace("&number=48", "&numbers=");
	}
	else
	{
		emit finished(false);
		return;
	}

	Http.setHost("www.eraomnix.pl");
	Http.setAgent("Kadu");
	Http.get(path + get_data, false);

	kdebugf2();
}

void SmsEraGateway::httpRedirected(QString link)
{
	kdebugmf(KDEBUG_FUNCTION_START, "link: %s\n", qPrintable(link));

	if (link.indexOf("localhost") > 0)
	{
		// remove unused parts of link
		link.remove("http://localhost?X-ERA-error=");
		link.remove(link.indexOf("X-ERA-counter="), 14);

		// split parts of link
		QStringList counters = link.split('&');
		int error = (*(counters.begin())).toInt();

		if (error == 0)
		{
			if (config_file.readEntry("SMS", "EraGateway") == "Sponsored")
				QMessageBox::information(0, "SMS", tr("Number of SMS' left on Sponsored Era Gateway: ") + counters.back(), QMessageBox::Ok);
			emit finished(true);
		}
		else
		{
			QMessageBox::critical(0, "SMS", tr("Error: ") + SmsEraGateway::errorNumber(error));
			emit finished(false);
		}
	}
	else
		QMessageBox::critical(0, "SMS", tr("Provider gateway results page looks strange. SMS was probably NOT sent."));

	kdebugf2();
}

// TODO: WTF is that, co za nazwa dla funkcji... kurde... kto to pisal?
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

void SmsEraGateway::createDefaultConfiguration()
{
	config_file.addVariable("SMS", "EraGateway", "Sponsored");
}

DefaultSmsConfigurationUiHandler::DefaultSmsConfigurationUiHandler(QObject *parent) :
		QObject(parent), era_types(), era_values(), actualEraGateway()
{
	kdebugf();

	kdebugf2();
}


void DefaultSmsConfigurationUiHandler::onChangeEraGateway()
{
	QString gateway = eraGateway->currentItemValue();
	if (gateway == "Sponsored")
	{
		sponsoredUser->show();
		sponsoredPassword->show();
		multimediaUser->hide();
		multimediaPassword->hide();
	}
	else
	{
		sponsoredUser->hide();
		sponsoredPassword->hide();
		multimediaUser->show();
		multimediaPassword->show();
	}
}

void DefaultSmsConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	eraGateway = dynamic_cast<ConfigComboBox *>(mainConfigurationWindow->widget()->widgetById("default_sms/eraGateway"));
	sponsoredUser = dynamic_cast<ConfigLineEdit *>(mainConfigurationWindow->widget()->widgetById("default_sms/sponsoredUser"));
	sponsoredPassword = dynamic_cast<ConfigLineEdit *>(mainConfigurationWindow->widget()->widgetById("default_sms/sponsoredPassword"));
	multimediaUser = dynamic_cast<ConfigLineEdit *>(mainConfigurationWindow->widget()->widgetById("default_sms/multimediaUser"));
	multimediaPassword = dynamic_cast<ConfigLineEdit *>(mainConfigurationWindow->widget()->widgetById("default_sms/multimediaPassword"));

	sponsoredPassword->setEchoMode(QLineEdit::Password);
	multimediaPassword->setEchoMode(QLineEdit::Password);

	connect(eraGateway, SIGNAL(activated(int)), this, SLOT(onChangeEraGateway()));
}

DefaultSmsConfigurationUiHandler* defaultSmsConfigurationUiHandler;

/** @} */

