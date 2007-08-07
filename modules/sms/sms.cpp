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

#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "../history/history.h"
#include "hot_key.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.cpp"
#include "modules.h"
#include "sms.h"
#include "userbox.h"

/**
 * @ingroup sms
 * @{
 */
extern "C" int sms_init()
{
	kdebugf();

	smsConfigurationUiHandler = new SmsConfigurationUiHandler();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/sms.ui"), smsConfigurationUiHandler);

	QObject::connect(kadu->userbox(), SIGNAL(doubleClicked(UserListElement)),
			smsConfigurationUiHandler, SLOT(onUserDblClicked(UserListElement)));
	QObject::connect(kadu->userbox(), SIGNAL(mouseButtonClicked(int, QListBoxItem*,const QPoint&)),
			smsConfigurationUiHandler, SLOT(onUserClicked(int, QListBoxItem*, const QPoint&)));
	QObject::connect(kadu->userbox(), SIGNAL(returnPressed(UserListElement)),
			smsConfigurationUiHandler, SLOT(onUserDblClicked(UserListElement)));
	QObject::connect(UserBox::userboxmenu, SIGNAL(popup()), smsConfigurationUiHandler, SLOT(onPopupMenuCreate()));

	config_file.addVariable("SMS", "Priority", QString::null);
	kdebugf2();
	return 0;
}

extern "C" void sms_close()
{
	kdebugf();

	QObject::disconnect(kadu->userbox(), SIGNAL(doubleClicked(UserListElement)),
			smsConfigurationUiHandler, SLOT(onUserDblClicked(UserListElement)));
	QObject::disconnect(kadu->userbox(), SIGNAL(returnPressed(UserListElement)),
			smsConfigurationUiHandler, SLOT(onUserDblClicked(UserListElement)));
	QObject::disconnect(kadu->userbox(), SIGNAL(mouseButtonClicked(int, QListBoxItem*,const QPoint&)),
			smsConfigurationUiHandler, SLOT(onUserClicked(int, QListBoxItem*, const QPoint&)));
	QObject::disconnect(UserBox::userboxmenu, SIGNAL(popup()), smsConfigurationUiHandler, SLOT(onPopupMenuCreate()));

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/sms.ui"), smsConfigurationUiHandler);
	delete smsConfigurationUiHandler;
	smsConfigurationUiHandler = 0;

	kdebugf2();
}

/********** SmsImageDialog **********/

SmsImageDialog::SmsImageDialog(QWidget* parent, const QByteArray& image)
	: QDialog(parent, "SmsImageDialog"), code_edit(0)
{
	kdebugf();

	ImageWidget *image_widget = new ImageWidget(this, image);
	QLabel* label=new QLabel(tr("Enter text from the picture:"), this);
	code_edit=new QLineEdit(this);

	QGridLayout *grid = new QGridLayout(this, 3, 2, 10, 10);
	grid->addMultiCellWidget(image_widget, 0, 0, 0, 1);
	grid->addWidget(label, 1, 0);
	grid->addWidget(code_edit, 1, 1);
	QHBox *buttons = new QHBox(this);
	buttons->setSpacing(10);
	QPushButton *okButton = new QPushButton(tr("Ok"), buttons);
	QPushButton *cancelButton = new QPushButton(tr("Cancel"), buttons);
	grid->addMultiCellWidget(buttons, 2, 2, 0, 1);

	connect(code_edit, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
	connect(okButton, SIGNAL(clicked()), this, SLOT(onReturnPressed()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	kdebugf2();
}

void SmsImageDialog::reject()
{
	kdebugf();
	emit codeEntered(QString::null);
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
	: QObject(parent, name), State(SMS_LOADING_PAGE), Number(), Signature(), Message(), Http()
{
	QObject::connect(&Http, SIGNAL(finished()), this, SLOT(httpFinished()));
	QObject::connect(&Http, SIGNAL(redirected(QString)), this, SLOT(httpRedirected(QString)));
	QObject::connect(&Http, SIGNAL(error()), this, SLOT(httpError()));
}

void SmsGateway::httpError()
{
	kdebugf();
	MessageBox::msg(tr("Network error. Provider gateway page is probably unavailable"), false, "Warning", (QDialog*)(parent()->parent()));
	emit finished(false);
	kdebugf2();
}

/********** SmsSender **********/

SmsSender::SmsSender(QObject* parent, const char *name)
	: QObject(parent, name), Gateway(0)
{
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
		MessageBox::msg(tr("Mobile number is incorrect"), false, "Warning", (QWidget*)parent());
		emit finished(false);
		kdebugf2();
		return;
	}
	if (signature.isEmpty())
	{
		MessageBox::msg(tr("Signature can't be empty"), false, "Warning", (QWidget*)parent());
		emit finished(false);
		kdebugf2();
		return;
	}
	Gateway=smsConfigurationUiHandler->getGateway(Number);

	if (Gateway==NULL)
	{
		MessageBox::msg(tr("Mobile number is incorrect or gateway is not available"), false, "Warning", (QWidget*)parent());
		emit finished(false);
		kdebugf2();
		return;
	}

	QObject::connect(Gateway, SIGNAL(finished(bool)), this, SLOT(onFinished(bool)));
	Gateway->send(Number, message, contact, signature);
	kdebugf2();
}

/********** Sms **********/

Sms::Sms(const QString& altnick, QWidget* parent, const char *name) : QWidget(parent, name, WType_TopLevel | WDestructiveClose),
	body(0), recipient(0), list(0), smslen(0), l_contact(0), e_contact(0), l_signature(0),
	e_signature(0), b_send(0), c_saveInHistory(0), smsProcess(0), Sender()
{
	kdebugf();

	QGridLayout *grid = new QGridLayout(this, 3, 4, 10, 7);

	body = new QMultiLineEdit(this);
	grid->addMultiCellWidget(body, 1, 1, 0, 3);
	body->setWordWrap(QMultiLineEdit::WidgetWidth);
	body->setTabChangesFocus(true);
	connect(body, SIGNAL(textChanged()), this, SLOT(updateCounter()));

	recipient = new QLineEdit(this);
	recipient->setMinimumWidth(140);
	if (altnick.isEmpty())
		recipient->setFocus();
	else
		recipient->setText(userlist->byAltNick(altnick).mobile());
	connect(recipient, SIGNAL(textChanged(const QString&)), this, SLOT(updateList(const QString&)));
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

	l_contact= new QLabel(tr("Contact"), this);
	grid->addWidget(l_contact, 3, 0);
	e_contact= new QLineEdit(this);
	grid->addWidget(e_contact, 3, 1);

	smslen = new QLabel("0", this);
	grid->addWidget(smslen, 3, 3, Qt::AlignRight);

	l_signature= new QLabel(tr("Signature"), this);
	grid->addWidget(l_signature, 4, 0);
	e_signature= new QLineEdit(config_file.readEntry("SMS", "SmsNick"), this);
	grid->addWidget(e_signature, 4, 1);

	c_saveInHistory = new QCheckBox(tr("Save SMS in history"), this);
	c_saveInHistory->setChecked(true);
	grid->addMultiCellWidget(c_saveInHistory, 5, 5, 0, 1);

	b_send = new QPushButton(this);
	b_send->setIconSet(icons_manager->loadIcon("SendSMSButton"));
	b_send->setText(tr("&Send"));
	b_send->setDefault(true);
	b_send->setMaximumWidth(200);
	connect(b_send, SIGNAL(clicked()), this, SLOT(sendSms()));
	grid->addWidget(b_send, 5, 3, Qt::AlignRight);

	resize(400, 250);
	setCaption(tr("Send SMS"));

	connect(&Sender, SIGNAL(finished(bool)), this, SLOT(onSmsSenderFinished(bool)));

	configurationUpdated();

	modules_manager->moduleIncUsageCount("sms");
	kdebugf2();
}

Sms::~Sms()
{
	modules_manager->moduleDecUsageCount("sms");
}

void Sms::configurationUpdated()
{
	body->setFont(config_file.readFontEntry("Look","ChatFont"));
}

void Sms::setRecipient(const QString &phone)
{
    kdebugf();
    recipient->setText(phone);
    body->setFocus();
    kdebugf2();
}

void Sms::updateRecipient(const QString &newtext)
{
	kdebugf();
//	kdebugmf(KDEBUG_FUNCTION_START | KDEBUG_INFO, "'%s' %d %d\n", newtext.local8Bit().data(), newtext.isEmpty(), userlist->containsAltNick(newtext));
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
	if (newnumber.isEmpty())
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: new number is empty\n");
		return;
	}
	CONST_FOREACH(user, *userlist)
		if ((*user).mobile() == newnumber)
		{
			list->setCurrentText((*user).altNick());
			kdebugf2();
			return;
		}
	list->setCurrentText(QString::null);
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
			MessageBox::msg(tr("Sms application was not specified. Visit the configuration section"), false, "Warning", this);
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
			MessageBox::msg(tr("Could not spawn child process. Check if the program is functional"), false, "Warning", this);
		QObject::connect(smsProcess, SIGNAL(processExited()), this, SLOT(smsSigHandler()));
	}
	kdebugf2();
}

void Sms::smsSigHandler()
{
	kdebugf();
	if (smsProcess->normalExit())
		MessageBox::msg(tr("The process exited normally. The SMS should be on its way"), false, "Information", this);
	else
		MessageBox::msg(tr("The process exited abnormally. The SMS may not be sent"), false, "Warning", this);
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
			history->appendSms(recipient->text(), body->text());
		if (!MessageBox::ask(tr("The SMS was sent and should be on its way.\nDo you want to send next message?"), "Information", this))
			deleteLater();
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

SmsConfigurationUiHandler::SmsConfigurationUiHandler()
	: menuid(0), gateways()
{
	kdebugf();
	UserBox::userboxmenu->addItemAtPos(2, "SendSms", tr("Send SMS"), this, SLOT(onSendSmsToUser()),
                HotKey::shortCutFromFile("ShortCuts", "kadu_sendsms"));

	QPopupMenu *MainMenu = kadu->mainMenu();
	menuid = MainMenu->insertItem(icons_manager->loadIcon("SendSms"), tr("Send SMS"), this, SLOT(onSendSms()), 0, -1, 10);
	icons_manager->registerMenuItem(MainMenu, tr("Send SMS"), "SendSms");

	Action* send_sms_action = new Action("SendSms", tr("Send SMS"), "sendSmsAction", Action::TypeGlobal);
	connect(send_sms_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(sendSmsActionActivated(const UserGroup*)));
	KaduActions.insert("sendSmsAction", send_sms_action);

	kdebugf2();
}

SmsConfigurationUiHandler::~SmsConfigurationUiHandler()
{
	kdebugf();
	int sendsmstem = UserBox::userboxmenu->getItem(tr("Send SMS"));
	UserBox::userboxmenu->removeItem(sendsmstem);
	kadu->mainMenu()->removeItem(menuid);
	KaduActions.remove("sendSmsAction");
	kdebugf2();
}


void SmsConfigurationUiHandler::onSmsBuildInCheckToggle(bool value)
{
	if (value)
	{
		customApp->setEnabled(false);
		useCustomString->setEnabled(false);
		customString->setEnabled(false);
	}
	else
	{
		customApp->setEnabled(true);
		useCustomString->setEnabled(true);
		customString->setEnabled(useCustomString->isChecked());
	}
}

void SmsConfigurationUiHandler::configurationUpdated()
{
	QStringList priority;
	QListBoxItem *item = gatewayListBox->firstItem();

	while (item)
	{
		priority += item->text();
		item = item->next();
	}

	// FIXME: : instead of ;
	config_file.writeEntry("SMS", "Priority", priority.join(";"));
}

void SmsConfigurationUiHandler::newSms(QString nick)
{
	(new Sms(nick, kadu, "sms"))->show();
}

void SmsConfigurationUiHandler::onUserClicked(int button, QListBoxItem* /*item*/, const QPoint& /*pos*/)
{
	if (button==4)
		onSendSmsToUser();
}

void SmsConfigurationUiHandler::onUserDblClicked(UserListElement user)
{
	kdebugf();
	if ((user.ID("Gadu") == kadu->myself().ID("Gadu") || !user.usesProtocol("Gadu")) && !user.mobile().isEmpty())
		newSms(user.altNick());
	kdebugf2();
}

void SmsConfigurationUiHandler::onSendSmsToUser()
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

void SmsConfigurationUiHandler::onSendSms()
{
	newSms(QString::null);
}

void SmsConfigurationUiHandler::registerGateway(QString name, isValidFunc* f)
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

void SmsConfigurationUiHandler::unregisterGateway(QString name)
{
	kdebugf();
	gateways.remove(name);
	kdebugf2();
}

SmsGateway* SmsConfigurationUiHandler::getGateway(const QString& number)
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

void SmsConfigurationUiHandler::onUpButton()
{
	int index = gatewayListBox->currentItem();
	if (index == 0)
		return;

	QString text = gatewayListBox->text(index);
	gatewayListBox->removeItem(index);
	gatewayListBox->insertItem(text, --index);
	gatewayListBox->setSelected(gatewayListBox->findItem(text), true);
}

void SmsConfigurationUiHandler::onDownButton()
{
	unsigned int index = gatewayListBox->currentItem();
	if (index == gatewayListBox->count())
		return;

	QString text = gatewayListBox->text(index);
	gatewayListBox->removeItem(index);
	gatewayListBox->insertItem(text, ++index);
	gatewayListBox->setSelected(gatewayListBox->findItem(text), true);
}

void SmsConfigurationUiHandler::onPopupMenuCreate()
{
	kdebugf();

	UserBox *activeUserBox = kadu->userbox()->activeUserBox();
	if (activeUserBox == NULL) //to sie zdarza...
		return;

	UserListElements users = activeUserBox->selectedUsers();
	if ((users[0]).mobile().isEmpty() || users.count() != 1)
		UserBox::userboxmenu->setItemVisible(UserBox::userboxmenu->getItem(tr("Send SMS")), false);

	kdebugf2();
}

void SmsConfigurationUiHandler::sendSmsActionActivated(const UserGroup* users)
{
	kdebugf();

	if (users != NULL && users->count() == 1 && !(*users->begin()).mobile().isEmpty())
		newSms((*users->begin()).altNick());
	else
		newSms(QString::null);

	kdebugf2();
}

void SmsConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	useBuiltIn = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widgetById("sms/useBuildInApp"));
	customApp = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("sms/customApp"));
	useCustomString = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widgetById("sms/useCustomString"));
	customString = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("sms/customString"));

	ConfigGroupBox *gatewayGroupBox = mainConfigurationWindow->configGroupBox("SMS", "General", "Gateways");
	QHBox *gatewayHBox = new QHBox(gatewayGroupBox->widget());
	gatewayHBox->setSpacing(5);

	gatewayListBox = new QListBox(gatewayHBox);

	QWidget *buttons = new QWidget(gatewayHBox);
	QVBoxLayout *buttonsLayout = new QVBoxLayout(buttons);
	buttonsLayout->setSpacing(5);

	QPushButton *up = new QPushButton(tr("Up"), buttons);
	QPushButton *down = new QPushButton(tr("Down"), buttons);

	buttonsLayout->addWidget(up);
	buttonsLayout->addWidget(down);
	buttonsLayout->addStretch(100);

	connect(up, SIGNAL(clicked()), this, SLOT(onUpButton()));
	connect(down, SIGNAL(clicked()), this, SLOT(onDownButton()));

	gatewayGroupBox->addWidgets(new QLabel(tr("Gateways priorites"), gatewayGroupBox->widget()), gatewayHBox);
	
	connect(useBuiltIn, SIGNAL(toggled(bool)), this, SLOT(onSmsBuildInCheckToggle(bool)));
	connect(useCustomString, SIGNAL(toggled(bool)), customString, SLOT(setEnabled(bool)));

// TODO: fix it, should be ':' not ';'
	QStringList priority = QStringList::split(";", config_file.readEntry("SMS", "Priority"));

	CONST_FOREACH(gate, priority)
		if (gateways.contains(*gate))
			gatewayListBox->insertItem(*gate);

	CONST_FOREACH(gate, gateways)
		if (gatewayListBox->index(gatewayListBox->findItem(gate.key())) == -1)
			gatewayListBox->insertItem(gate.key());
}

SmsConfigurationUiHandler *smsConfigurationUiHandler;

/** @} */

