/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcheckbox.h>
#include <qgrid.h>
#include <qlayout.h>
#include <qstring.h>

#include "config_dialog.h"
#include "config_file.h"
#include "history.h"
#include "icons_manager.h"
#include "kadu.h"
#include "modules.h"
#include "sms.h"
#include "userbox.h"

extern "C" int sms_init()
{
	kdebugf();

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "SMS"), "SMSTab");
	ConfigDialog::addVGroupBox("SMS", "SMS",
			QT_TRANSLATE_NOOP("@default", "SMS options"), "", Expert);
	ConfigDialog::addCheckBox("SMS", "SMS options",
			QT_TRANSLATE_NOOP("@default", "Use built-in SMS application"), "BuiltInApp", true);
	ConfigDialog::addLineEdit("SMS", "SMS options",
			QT_TRANSLATE_NOOP("@default", "Custom SMS application"), "SmsApp");
	ConfigDialog::addGrid("SMS", "SMS options", "smsgrid", 2);
	ConfigDialog::addCheckBox("SMS", "smsgrid",
			QT_TRANSLATE_NOOP("@default", "SMS custom string"), "UseCustomString", false ,
			QT_TRANSLATE_NOOP("@default", "Check this box if your sms application doesn't understand arguments: number \"message\"\nArguments should be separated with spaces. %n argument is converted to number, %m to message"));
	ConfigDialog::addLineEdit("SMS", "smsgrid", "", "SmsString", "", "", "smsstring");
	ConfigDialog::addHGroupBox("SMS", "SMS",
			QT_TRANSLATE_NOOP("@default", "Gateways priority"), "", Advanced);
	ConfigDialog::addListBox("SMS", "Gateways priority", "gateways");
	ConfigDialog::addVBox("SMS", "Gateways priority", "button");
	ConfigDialog::addPushButton("SMS", "button", QT_TRANSLATE_NOOP("@default", "Up"));
	ConfigDialog::addPushButton("SMS", "button", QT_TRANSLATE_NOOP("@default", "Down"));
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys",
			QT_TRANSLATE_NOOP("@default", "Send SMS"), "kadu_sendsms", "Ctrl+S");


	smsslots=new SmsSlots(NULL, "smsslots");
	ConfigDialog::registerSlotOnCreateTab("SMS", smsslots, SLOT(onCreateTabSMS()));
	ConfigDialog::registerSlotOnCloseTab("SMS", smsslots, SLOT(onCloseTabSMS()));
	ConfigDialog::registerSlotOnApplyTab("SMS", smsslots, SLOT(onApplyTabSMS()));

	ConfigDialog::connectSlot("SMS", "Use built-in SMS application", SIGNAL(toggled(bool)), smsslots, SLOT(onSmsBuildInCheckToggle(bool)));
	ConfigDialog::connectSlot("SMS", "Up", SIGNAL(clicked()), smsslots, SLOT(onUpButton()));
	ConfigDialog::connectSlot("SMS", "Down", SIGNAL(clicked()), smsslots, SLOT(onDownButton()));

	QObject::connect(kadu->userbox(), SIGNAL(doubleClicked(UserListElement)),
			smsslots, SLOT(onUserDblClicked(UserListElement)));
	QObject::connect(kadu->userbox(), SIGNAL(mouseButtonClicked(int, QListBoxItem*,const QPoint&)),
			smsslots, SLOT(onUserClicked(int, QListBoxItem*, const QPoint&)));
	QObject::connect(kadu->userbox(), SIGNAL(returnPressed(UserListElement)),
			smsslots, SLOT(onUserDblClicked(UserListElement)));
	QObject::connect(UserBox::userboxmenu, SIGNAL(popup()), smsslots, SLOT(onPopupMenuCreate()));

	config_file.addVariable("SMS", "Priority", "");
	kdebugf2();
	return 0;
}

extern "C" void sms_close()
{
	kdebugf();

	ConfigDialog::unregisterSlotOnCreateTab("SMS", smsslots, SLOT(onCreateTabSMS()));
	ConfigDialog::unregisterSlotOnCloseTab("SMS", smsslots, SLOT(onCloseTabSMS()));
	ConfigDialog::unregisterSlotOnApplyTab("SMS", smsslots, SLOT(onApplyTabSMS()));

	ConfigDialog::disconnectSlot("SMS", "Use built-in SMS application", SIGNAL(toggled(bool)), smsslots, SLOT(onSmsBuildInCheckToggle(bool)));
	ConfigDialog::disconnectSlot("SMS", "Up", SIGNAL(clicked()), smsslots, SLOT(onUpButton()));
	ConfigDialog::disconnectSlot("SMS", "Down", SIGNAL(clicked()), smsslots, SLOT(onDownButton()));

	ConfigDialog::removeControl("ShortCuts", "Send SMS");
	ConfigDialog::removeControl("SMS", "Up");
	ConfigDialog::removeControl("SMS", "Down");
	ConfigDialog::removeControl("SMS", "button");
	ConfigDialog::removeControl("SMS", "gateways");
	ConfigDialog::removeControl("SMS", "Gateways priority");
	ConfigDialog::removeControl("SMS", "", "smsstring");
	ConfigDialog::removeControl("SMS", "SMS custom string");
	ConfigDialog::removeControl("SMS", "smsgrid");
	ConfigDialog::removeControl("SMS", "Custom SMS application");
	ConfigDialog::removeControl("SMS", "Use built-in SMS application");
	ConfigDialog::removeControl("SMS", "SMS options");
	ConfigDialog::removeTab("SMS");

	QObject::disconnect(kadu->userbox(), SIGNAL(doubleClicked(UserListElement)),
			smsslots, SLOT(onUserDblClicked(UserListElement)));
	QObject::disconnect(kadu->userbox(), SIGNAL(returnPressed(UserListElement)),
			smsslots, SLOT(onUserDblClicked(UserListElement)));
	QObject::disconnect(kadu->userbox(), SIGNAL(mouseButtonClicked(int, QListBoxItem*,const QPoint&)),
			smsslots, SLOT(onUserClicked(int, QListBoxItem*, const QPoint&)));
	QObject::disconnect(UserBox::userboxmenu, SIGNAL(popup()), smsslots, SLOT(onPopupMenuCreate()));

	delete smsslots;
	smsslots=NULL;
	kdebugf2();
}

/********** SmsImageDialog **********/

SmsImageDialog::SmsImageDialog(QDialog* parent, const QByteArray& image)
	: QDialog (parent, "SmsImageDialog")
{
	kdebugf();

	ImageWidget *image_widget = new ImageWidget(this, image);
	QLabel* label=new QLabel(tr("Enter text from the picture:"), this);
	code_edit=new QLineEdit(this);

	QGridLayout *grid = new QGridLayout(this, 2, 2, 10, 10);
	grid->addMultiCellWidget(image_widget, 0, 0, 0, 1);
	grid->addWidget(label, 1, 0);
	grid->addWidget(code_edit, 1, 1);

	connect(code_edit, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
	kdebugf2();
}

void SmsImageDialog::reject()
{
	kdebugf();
	emit codeEntered("");
	QDialog::reject();
	kdebugf2();
}

void SmsImageDialog::onReturnPressed()
{
	kdebugf();
	accept();
	emit codeEntered(code_edit->text());
	kdebugf2();
}

/********** SmsGateway **********/

SmsGateway::SmsGateway(QObject* parent, const char *name)
	: QObject(parent, name)
{
	QObject::connect(&Http, SIGNAL(finished()), this, SLOT(httpFinished()));
	QObject::connect(&Http, SIGNAL(redirected(QString)), this, SLOT(httpRedirected(QString)));
	QObject::connect(&Http, SIGNAL(error()), this, SLOT(httpError()));
}

void SmsGateway::httpError()
{
	kdebugf();
	QMessageBox::critical((QDialog*)(parent()->parent()), "SMS", tr("Network error. Provider gateway page is probably unavailable"));
	emit finished(false);
	kdebugf2();
}

/********** SmsSender **********/

SmsSender::SmsSender(QObject* parent, const char *name)
	: QObject(parent, name)
{
	Gateway=NULL;
}

SmsSender::~SmsSender()
{
	kdebugf();
	emit finished(false);
	if (Gateway)
	{
		QObject::disconnect(Gateway, SIGNAL(finished(bool)), this, SLOT(onFinished(bool)));
		delete Gateway;
	}
	kdebugf2();
}

void SmsSender::onFinished(bool success)
{
	emit finished(success);
}

void SmsSender::send(const QString& number,const QString& message, const QString& contact, const QString& signature)
{
	kdebugf();
	QString Number=number;
	if (Number.length()==12 && Number.left(3)=="+48")
		Number=Number.right(9);
	if (Number.length()!=9)
	{
		QMessageBox::critical((QWidget*)parent(), "SMS", tr("Mobile number is incorrect"));
		emit finished(false);
		kdebugf2();
		return;
	}
	if (signature.isEmpty())
	{
		QMessageBox::critical((QWidget*)parent(), "SMS", tr("Signature can't be empty"));
		emit finished(false);
		kdebugf2();
		return;
	}
	Gateway=smsslots->getGateway(Number);

	if (Gateway==NULL)
	{
		QMessageBox::critical((QWidget*)parent(),"SMS", tr("Mobile number is incorrect or gateway is not available"));
		emit finished(false);
		kdebugf2();
		return;
	}

	QObject::connect(Gateway, SIGNAL(finished(bool)), this, SLOT(onFinished(bool)));
	Gateway->send(Number, message, contact, signature);
	kdebugf2();
}

/********** Sms **********/

Sms::Sms(const QString& altnick, QDialog* parent, const char *name) : QDialog (parent, name)
{
	kdebugf();
	QGridLayout * grid = new QGridLayout(this, 3, 4, 10, 3);

	setWFlags(WDestructiveClose);

	body = new QMultiLineEdit(this);
	grid->addMultiCellWidget(body, 1, 1, 0, 3);
	body->setWordWrap(QMultiLineEdit::WidgetWidth);
	body->setFont(config_file.readFontEntry("Look","ChatFont"));
	QObject::connect(body, SIGNAL(textChanged()), this, SLOT(updateCounter()));

	recipient = new QLineEdit(this);
	recipient->setMinimumWidth(140);
	if (!altnick.isEmpty())
		recipient->setText(userlist->byAltNick(altnick).mobile());
	QObject::connect(recipient, SIGNAL(textChanged(const QString&)), this, SLOT(updateList(const QString&)));
	grid->addWidget(recipient, 0, 1);

	QStringList strlist;
	list = new QComboBox(this);
	CONST_FOREACH(user, *userlist)
		if (!(*user).mobile().isEmpty())
		 	strlist.append((*user).altNick());
	strlist.sort();
	strlist.insert(strlist.begin(), QString::null);
	list->insertStringList(strlist);
	list->setCurrentText(altnick);
	connect(list, SIGNAL(activated(const QString&)), this, SLOT(updateRecipient(const QString &)));
	grid->addWidget(list, 0, 3);

	QLabel *recilabel = new QLabel(tr("Recipient"), this);
	grid->addWidget(recilabel, 0, 0);

	smslen = new QLabel("0", this);
	grid->addWidget(smslen, 3, 0);
	c_saveInHistory = new QCheckBox(tr("Save SMS in history"), this);
	c_saveInHistory->setChecked(true);
	grid->addWidget(c_saveInHistory, 3, 1);

	b_send = new QPushButton(this);
	b_send->setText(tr("Send"));
	b_send->setMaximumWidth(200);
	grid->addWidget(b_send, 3, 3, Qt::AlignRight);

	l_contact= new QLabel(tr("Contact"), this);
	grid->addWidget(l_contact, 4, 0);
	e_contact= new QLineEdit(this);
	grid->addWidget(e_contact, 4, 1);

	l_signature= new QLabel(tr("Signature"), this);
	grid->addWidget(l_signature, 5, 0);
	e_signature= new QLineEdit(config_file.readEntry("General", "Nick"), this);
	grid->addWidget(e_signature, 5, 1);

	if (altnick.isEmpty())
		recipient->setFocus();
	resize(400, 250);
	setCaption(tr("Send SMS"));

	connect(b_send, SIGNAL(clicked()), this, SLOT(sendSms()));
	connect(&Sender, SIGNAL(finished(bool)), this, SLOT(onSmsSenderFinished(bool)));

	modules_manager->moduleIncUsageCount("sms");
	kdebugf2();
}

Sms::~Sms()
{
	modules_manager->moduleDecUsageCount("sms");
}

void Sms::updateRecipient(const QString &newtext)
{
	kdebugf();
	if (newtext.isEmpty())
	{
		recipient->clear();
		kdebugf2();
		return;
	}
	if (userlist->containsAltNick(newtext))
		recipient->setText(userlist->byAltNick(newtext).mobile());
	kdebugf2();
}

void Sms::updateList(const QString &newnumber)
{
	kdebugf();
	CONST_FOREACH(user, *userlist)
		if ((*user).mobile() == newnumber)
		{
			list->setCurrentText((*user).altNick());
			kdebugf2();
			return;
		}
	list->setCurrentText("");
	kdebugf2();
}

void Sms::sendSms(void)
{
	kdebugf();
	b_send->setEnabled(false);
	body->setEnabled(false);
	e_contact->setEnabled(false);
	l_contact->setEnabled(false);
	e_signature->setEnabled(false);
	l_signature->setEnabled(false);
	c_saveInHistory->setEnabled(false);

	if (config_file.readBoolEntry("SMS","BuiltInApp"))
	{
		Sender.send(recipient->text(), body->text(), e_contact->text(), e_signature->text());
	}
	else
	{
		if (config_file.readEntry("SMS","SmsApp").isEmpty())
		{
			QMessageBox::warning(this, tr("SMS error"), tr("Sms application was not specified. Visit the configuration section") );
			kdebugm(KDEBUG_WARNING, "SMS application NOT specified. Exit.\n");
			return;
		}
		QString SmsAppPath=config_file.readEntry("SMS","SmsApp");

		smsProcess = new QProcess(this);
		if (config_file.readBoolEntry("SMS","UseCustomString")&&
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
		}

		if (!smsProcess->start())
			QMessageBox::critical(this, tr("SMS error"), tr("Could not spawn child process. Check if the program is functional") );
		QObject::connect(smsProcess, SIGNAL(processExited()), this, SLOT(smsSigHandler()));
	}
	kdebugf2();
}

void Sms::smsSigHandler()
{
	kdebugf();
	if (smsProcess->normalExit())
		QMessageBox::information(this, tr("SMS sent"), tr("The process exited normally. The SMS should be on its way"));
	else
		QMessageBox::warning(this, tr("SMS not sent"), tr("The process exited abnormally. The SMS may not be sent"));
	delete smsProcess;
	smsProcess = NULL;

	c_saveInHistory->setEnabled(true);
	e_contact->setEnabled(true);
	l_contact->setEnabled(true);
	e_signature->setEnabled(true);
	l_signature->setEnabled(true);
	b_send->setEnabled(true);
	body->setEnabled(true);
	body->clear();
	kdebugf2();
}

void Sms::updateCounter()
{
	smslen->setText(QString::number(body->text().length()));
}

void Sms::onSmsSenderFinished(bool success)
{
	kdebugf();
	if (success)
	{
		if (c_saveInHistory->isChecked())
			history.appendSms(recipient->text(), body->text());
		QMessageBox::information(this, tr("SMS sent"), tr("The SMS was sent and should be on its way"));
		body->clear();
	}
	b_send->setEnabled(true);
	body->setEnabled(true);
	e_contact->setEnabled(true);
	l_contact->setEnabled(true);
	e_signature->setEnabled(true);
	l_signature->setEnabled(true);
	c_saveInHistory->setEnabled(true);
	kdebugf2();
}

SmsSlots::SmsSlots(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	UserBox::userboxmenu->addItemAtPos(2, "SendSms", tr("Send SMS"), this, SLOT(onSendSmsToUser()),
                HotKey::shortCutFromFile("ShortCuts", "kadu_sendsms"));

	menuid=kadu->mainMenu()->insertItem(icons_manager->loadIcon("SendSms"), tr("Send SMS"), this, SLOT(onSendSms()), 0, -1, 10);
	icons_manager->registerMenuItem(kadu->mainMenu(), tr("Send SMS"), "SendSms");
	kdebugf2();
}

SmsSlots::~SmsSlots()
{
	kdebugf();
	int sendsmstem = UserBox::userboxmenu->getItem(tr("Send SMS"));
	UserBox::userboxmenu->removeItem(sendsmstem);
	kadu->mainMenu()->removeItem(menuid);
	kdebugf2();
}


void SmsSlots::onSmsBuildInCheckToggle(bool value)
{
	kdebugf();

	QLineEdit *e_smsapp= ConfigDialog::getLineEdit("SMS", "Custom SMS application");
	QCheckBox *b_smscustomconf= ConfigDialog::getCheckBox("SMS", "SMS custom string");
	QLineEdit *e_smsconf= ConfigDialog::getLineEdit("SMS","","smsstring");

	((QHBox*)(e_smsapp->parent()))->setEnabled(!value);
	b_smscustomconf->setEnabled(!value);
	e_smsconf->setEnabled(b_smscustomconf->isChecked()&& !value);
	kdebugf2();
}

void SmsSlots::onCreateTabSMS()
{
	kdebugf();

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

	QListBox* lb_gws=ConfigDialog::getListBox("SMS", "gateways");
//	QMap<QString,isValidFunc*>::Iterator it;
	QStringList priority=QStringList::split(";", config_file.readEntry("SMS", "Priority"));

	CONST_FOREACH(gate, priority)
		if (gateways.contains(*gate))
			lb_gws->insertItem(*gate);

	CONST_FOREACH(gate, gateways)
		if (lb_gws->index(lb_gws->findItem(gate.key()))==-1)
			lb_gws->insertItem(gate.key());

	modules_manager->moduleIncUsageCount("sms");
	kdebugf2();
}

void SmsSlots::onApplyTabSMS()
{
	kdebugf();

	QListBox* lb_gws=ConfigDialog::getListBox("SMS", "gateways");
	QStringList priority;
	QListBoxItem* lbi_item=lb_gws->firstItem();

	while (lbi_item!=0)
	{
		priority+=lbi_item->text();
		lbi_item=lbi_item->next();
	}

	config_file.writeEntry("SMS", "Priority", priority.join(";"));
	kdebugf2();
}

void SmsSlots::onCloseTabSMS()
{
	modules_manager->moduleDecUsageCount("sms");
}

void SmsSlots::newSms(QString nick)
{
	(new Sms(nick, NULL, "sms"))->show();
}

void SmsSlots::onUserClicked(int button, QListBoxItem* /*item*/, const QPoint& /*pos*/)
{
	if (button==4)
		onSendSmsToUser();
}

void SmsSlots::onUserDblClicked(UserListElement user)
{
	kdebugf();
	if (!user.usesProtocol("Gadu"))
		newSms(user.altNick());
	kdebugf2();
}

void SmsSlots::onSendSmsToUser()
{
	kdebugf();
	UserListElements users;
	UserBox *activeUserBox = kadu->userbox()->activeUserBox();
	if (activeUserBox==NULL)
		return;
	users = activeUserBox->selectedUsers();
	if (users.count() != 1)
		return;
	if (!users[0].mobile().isEmpty())
		newSms(users[0].altNick());
	kdebugf2();
}

void SmsSlots::onSendSms()
{
	newSms("");
}

void SmsSlots::registerGateway(QString name, isValidFunc* f)
{
	kdebugf();
	QStringList priority=QStringList::split(";", config_file.readEntry("SMS", "Priority"));
	if (!priority.contains(name))
	{
		priority+=name;
		config_file.writeEntry("SMS", "Priority", priority.join(";"));
	}
	gateways.insert(name, f);
	kdebugf2();
}

void SmsSlots::unregisterGateway(QString name)
{
	kdebugf();
	gateways.remove(name);
	kdebugf2();
}

SmsGateway* SmsSlots::getGateway(const QString& number)
{
	kdebugf();
	QStringList priorities = QStringList::split(";", config_file.readEntry("SMS", "Priority"));

	CONST_FOREACH(gate, priorities)
	{
		if (gateways.contains(*gate))
		{
			isValidFunc *f=gateways[*gate];
			SmsGateway *Gateway=f(number, this);
			if (Gateway)
			{
				kdebugf2();
				return Gateway;
			}
		}
	}

	kdebugmf(KDEBUG_INFO|KDEBUG_FUNCTION_END, "return NULL\n");
	return NULL;
}

void SmsSlots::onUpButton()
{
	kdebugf();
	QListBox* list=ConfigDialog::getListBox("SMS", "gateways");
	int index=list->currentItem();
	if (index==0)
		return;
	QString text=list->text(index);
	list->removeItem(index);
	list->insertItem(text, --index);
	list->setSelected(list->findItem(text), true);
	kdebugf2();
}

void SmsSlots::onDownButton()
{
	kdebugf();
	QListBox* list=ConfigDialog::getListBox("SMS", "gateways");
	unsigned int index=list->currentItem();
	if (index==list->count())
		return;
	QString text=list->text(index);
	list->removeItem(index);
	list->insertItem(text, ++index);
	list->setSelected(list->findItem(text), true);
	kdebugf2();
}

void SmsSlots::onPopupMenuCreate()
{
	kdebugf();
	UserListElements users;
	UserBox *activeUserBox = kadu->userbox()->activeUserBox();
	if (activeUserBox == NULL)//to siê zdarza...
		return;
	users = activeUserBox->selectedUsers();
	UserListElement user = users[0];

	if (user.mobile().isEmpty() || users.count() != 1)
		UserBox::userboxmenu->setItemEnabled(UserBox::userboxmenu->getItem(tr("Send SMS")), false);
	kdebugf2();
}

SmsSlots *smsslots;
