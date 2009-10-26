/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtCore/QProcess>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "debug.h"
#include "gui/widgets/contacts-list-widget.h"
#include "gui/widgets/contacts-list-view-menu-manager.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/image-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-box.h"
#include "gui/hot-key.h"
#include "icons-manager.h"

#include "modules.h"
#include "misc/path-conversion.h"


#include "../history/history.h"

#include "sms.h"

/**
 * @ingroup sms
 * @{
 */
extern "C" KADU_EXPORT int sms_init(bool firstLoad)
{
	kdebugf();

	smsConfigurationUiHandler = new SmsConfigurationUiHandler();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/sms.ui"));
	MainConfigurationWindow::registerUiHandler(smsConfigurationUiHandler);

	//QObject::connect(Core::instance()->kaduWindow()->contactsListView(), SIGNAL(chatActivated(Chat *)), smsConfigurationUiHandler, SLOT(onUserDblClicked(Chat *)));
	//QObject::connect(Core::instance()->kaduWindow()->contactsListView(), SIGNAL(currentContactChanged(Contact)), smsConfigurationUiHandler, SLOT(onUserClicked(Contact)));

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void sms_close()
{
	kdebugf();

	//QObject::disconnect(Core::instance()->kaduWindow()->contactsListView(), SIGNAL(chatActivated(Chat *)), smsConfigurationUiHandler, SLOT(onUserDblClicked(Chat *)));
	//QObject::disconnect(Core::instance()->kaduWindow()->contactsListView(), SIGNAL(currentContactChanged(Contact)), smsConfigurationUiHandler, SLOT(onUserClicked(Contact)));

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/sms.ui"));
	MainConfigurationWindow::unregisterUiHandler(smsConfigurationUiHandler);
	delete smsConfigurationUiHandler;
	smsConfigurationUiHandler = 0;

	kdebugf2();
}

/********** SmsImageDialog **********/

SmsImageDialog::SmsImageDialog(QWidget* parent, const QByteArray& image)
	: QDialog(parent), code_edit(0)
{
	kdebugf();

	ImageWidget *image_widget = new ImageWidget(image, this);
	QLabel* label = new QLabel(tr("Enter text from the picture:"), this);
	code_edit = new QLineEdit(this);

	QGridLayout *grid = new QGridLayout(this);
	grid->addWidget(image_widget, 0, 0, 0, 1);
	grid->addWidget(label, 1, 0);
	grid->addWidget(code_edit, 1, 1);
	
	QWidget *buttonsWidget = new QWidget(this);
	QHBoxLayout *buttonsLayout = new QHBoxLayout(buttonsWidget);

	QPushButton *okButton = new QPushButton(tr("Ok"), buttonsWidget);
	QPushButton *cancelButton = new QPushButton(tr("Cancel"), buttonsWidget);

	buttonsLayout->setSpacing(10);
	buttonsLayout->addWidget(okButton);
	buttonsLayout->addWidget(cancelButton);

	grid->addWidget(buttonsWidget, 2, 2, 0, 1);

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

SmsGateway::SmsGateway(QObject* parent)
	: QObject(parent), State(SMS_LOADING_PAGE), Number(), Signature(), Message(), Http()
{
	connect(&Http, SIGNAL(finished()), this, SLOT(httpFinished()));
	connect(&Http, SIGNAL(redirected(QString)), this, SLOT(httpRedirected(QString)));
	connect(&Http, SIGNAL(error()), this, SLOT(httpError()));
}

void SmsGateway::httpError()
{
	kdebugf();
	MessageBox::msg(tr("Network error. Provider gateway page is probably unavailable"), false, "Warning", (QDialog*)(parent()->parent()));
	emit finished(false);
	kdebugf2();
}

/********** SmsSender **********/

SmsSender::SmsSender(QObject* parent)
	: QObject(parent), Gateway(0)
{
}

SmsSender::~SmsSender()
{
	kdebugf();
	emit finished(false);
	if (Gateway)
	{
		disconnect(Gateway, SIGNAL(finished(bool)), this, SLOT(onFinished(bool)));
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
	if (Number.length() == 12 && Number.left(3) == "+48")
		Number=Number.right(9);
	if (Number.length() != 9)
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
	Gateway = smsConfigurationUiHandler->getGateway(Number);

	if (!Gateway)
	{
		MessageBox::msg(tr("Mobile number is incorrect or gateway is not available"), false, "Warning", (QWidget*)parent());
		emit finished(false);
		kdebugf2();
		return;
	}

	connect(Gateway, SIGNAL(finished(bool)), this, SLOT(onFinished(bool)));
	Gateway->send(Number, message, contact, signature);
	kdebugf2();
}

/********** Sms **********/

Sms::Sms(const QString& altnick, QWidget* parent) : QWidget(parent, Qt::Window),
	body(0), recipient(0), list(0), smslen(0), l_contact(0), e_contact(0), l_signature(0),
	e_signature(0), b_send(0), c_saveInHistory(0), smsProcess(0), Sender()
{
	kdebugf();

	setWindowTitle(tr("Send SMS"));
	setAttribute(Qt::WA_DeleteOnClose);

	QGridLayout *grid = new QGridLayout(this);
	grid->setColumnStretch(1, 5);

	body = new QTextEdit(this);
	grid->addWidget(body, 1, 0, 1, 3);
	body->setLineWrapMode(QTextEdit::WidgetWidth);
	body->setTabChangesFocus(true);
	connect(body, SIGNAL(textChanged()), this, SLOT(updateCounter()));

	recipient = new QLineEdit(this);
	recipient->setMinimumWidth(140);
	if (altnick.isEmpty())
		recipient->setFocus();
	else
		recipient->setText(ContactManager::instance()->byDisplay(altnick).mobile());
	connect(recipient, SIGNAL(textChanged(const QString&)), this, SLOT(updateList(const QString&)));
	connect(recipient, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));
	grid->addWidget(recipient, 0, 1, 1, 1);

	QStringList strlist; // lista kontaktow z przypisanym numerem telefonu
	foreach(Contact c, ContactManager::instance()->contacts())
		if (!c.mobile().isEmpty())
		 	strlist.append(c.display());
	strlist.sort();
	strlist.prepend(QString::null);

	list = new QComboBox(this);
	list->addItems(strlist);
	list->setCurrentIndex(list->findText(altnick));
	connect(list, SIGNAL(activated(const QString&)), this, SLOT(updateRecipient(const QString &)));
	grid->addWidget(list, 0, 2, 1, 1);

	QLabel *recilabel = new QLabel(tr("Recipient"), this);
	grid->addWidget(recilabel, 0, 0, 1, 1);

	l_contact = new QLabel(tr("Contact"), this);
	grid->addWidget(l_contact, 2, 0, 1, 1);
	e_contact = new QLineEdit(this);
	connect(e_contact, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));
	grid->addWidget(e_contact, 2, 1, 1, 1);

	smslen = new QLabel("0", this);
	grid->addWidget(smslen, 2, 2, 1, 1, Qt::AlignRight);

	l_signature = new QLabel(tr("Signature"), this);
	grid->addWidget(l_signature, 3, 0, 1, 1);
	e_signature = new QLineEdit(config_file.readEntry("SMS", "SmsNick"), this);
	connect(e_signature, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));
	grid->addWidget(e_signature, 3, 1, 1, 1);

	c_saveInHistory = new QCheckBox(tr("Save SMS in history"), this);
	c_saveInHistory->setChecked(true);
	grid->addWidget(c_saveInHistory, 4, 0, 1, 2);

	b_send = new QPushButton(this);
	b_send->setIcon(IconsManager::instance()->loadIcon("SendSMSButton"));
	b_send->setText(tr("&Send"));
	b_send->setDefault(true);
	b_send->setMaximumWidth(200);
	connect(b_send, SIGNAL(clicked()), this, SLOT(editReturnPressed()));
	grid->addWidget(b_send, 4, 2, 1, 1, Qt::AlignRight);

	resize(400, 250);

	connect(&Sender, SIGNAL(finished(bool)), this, SLOT(onSmsSenderFinished(bool)));

	configurationUpdated();
	
	///restoreGeometry(this, "Sms", "SmsDialogGeometry", 200, 200, 400, 250);

	ModulesManager::instance()->moduleIncUsageCount("sms");
	kdebugf2();
}

Sms::~Sms()
{
	///saveGeometry(this, "Sms", "SmsDialogGeometry");

	ModulesManager::instance()->moduleDecUsageCount("sms");
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
//	kdebugmf(KDEBUG_FUNCTION_START | KDEBUG_INFO, "'%s' %d %d\n", qPrintable(newtext), newtext.isEmpty(), userlist->containsAltNick(newtext));
	if (newtext.isEmpty())
	{
		recipient->clear();
		kdebugf2();
		return;
	}
	Contact c = ContactManager::instance()->byDisplay(newtext);
	if (!c.isNull())
		recipient->setText(c.mobile());
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
	foreach(Contact c, ContactManager::instance()->contacts())
		if (c.mobile() == newnumber)
		{
			list->setCurrentIndex(list->findText(c.display()));
			kdebugf2();
			return;
		}
	list->setCurrentIndex(-1);
	kdebugf2();
}

void Sms::editReturnPressed()
{
	kdebugf();

	if (body->toPlainText().isEmpty())
		body->setFocus();
	else
		sendSms();

	kdebugf2();
}

void Sms::sendSms()
{
	kdebugf();
	b_send->setEnabled(false);
	body->setEnabled(false);
	e_contact->setEnabled(false);
	l_contact->setEnabled(false);
	e_signature->setEnabled(false);
	l_signature->setEnabled(false);
	c_saveInHistory->setEnabled(false);

	if (config_file.readBoolEntry("SMS", "BuiltInApp"))
	{
		Sender.send(recipient->text(), body->toPlainText(), e_contact->text(), e_signature->text());
	}
	else
	{
		if (config_file.readEntry("SMS", "SmsApp").isEmpty())
		{
			MessageBox::msg(tr("Sms application was not specified. Visit the configuration section"), false, "Warning", this);
			kdebugm(KDEBUG_WARNING, "SMS application NOT specified. Exit.\n");
			return;
		}
		QString SmsAppPath = config_file.readEntry("SMS", "SmsApp");

		smsProcess = new QProcess(this);
		if (config_file.readBoolEntry("SMS", "UseCustomString")&&
			(!config_file.readBoolEntry("SMS", "BuiltInApp")))
		{
			QStringList args = config_file.readEntry("SMS", "SmsString").split(' ');

			//FIXME args.find -> args.filter - to samo na pewno?
			/*if(args.filter("%n") != args.end())
				*args.filter("%n") = recipient->text();
			if(args.filter("%m") != args.end())
				*args.filter("%m") = body->toPlainText();
			*/
			smsProcess->start(SmsAppPath, args);
		}
		else
		{
			QStringList args(recipient->text());
			args.append(body->toPlainText());
			smsProcess->start(SmsAppPath, args);
		}

		if (!smsProcess->waitForStarted())
			MessageBox::msg(tr("Could not spawn child process. Check if the program is functional"), false, "Warning", this);
		connect(smsProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(smsSigHandler()));
	}
	kdebugf2();
}

void Sms::smsSigHandler()
{
	kdebugf();
	if (smsProcess->exitStatus() == QProcess::NormalExit)
		MessageBox::msg(tr("The process exited normally. The SMS should be on its way"), false, "Information", this);
	else
		MessageBox::msg(tr("The process exited abnormally. The SMS may not be sent"), false, "Warning", this);
	delete smsProcess;
	smsProcess = 0;

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
	smslen->setText(QString::number(body->toPlainText().length()));
}

void Sms::onSmsSenderFinished(bool success)
{
	kdebugf();
	if (success)
	{
		if (c_saveInHistory->isChecked())
		//TODO 0.6.6
		///	history->appendSms(recipient->text(), body->text());
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

void Sms::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}

SmsConfigurationUiHandler::SmsConfigurationUiHandler()
	: menuid(0), gateways(), gatewayListWidget(0)
{
	kdebugf();

	createDefaultConfiguration();

	sendSmsActionDescription = new ActionDescription(this,
		ActionDescription::TypeGlobal, "sendSmsAction",
		this, SLOT(sendSmsActionActivated(QAction *, bool)),
		"SendSms", tr("Send SMS"), false
	);
	sendSmsActionDescription->setShortcut("kadu_sendsms");
	ContactsListViewMenuManager::instance()->insertActionDescription(2, sendSmsActionDescription);
	Core::instance()->kaduWindow()->insertMenuActionDescription(sendSmsActionDescription, KaduWindow::MenuContacts, 4);
	kdebugf2();
}

SmsConfigurationUiHandler::~SmsConfigurationUiHandler()
{
	kdebugf();
	ContactsListViewMenuManager::instance()->removeActionDescription(sendSmsActionDescription);
	Core::instance()->kaduWindow()->removeMenuActionDescription(sendSmsActionDescription);
	delete sendSmsActionDescription;
	sendSmsActionDescription = 0;
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
	if (!gatewayListWidget)
		return;

	QStringList priority;
	for (int index = 0; index < gatewayListWidget->count(); ++index)
			priority += gatewayListWidget->item(index)->text();

	// FIXME: : instead of ;
	config_file.writeEntry("SMS", "Priority", priority.join(";"));
}

void SmsConfigurationUiHandler::newSms(QString nick)
{
	(new Sms(nick/*, Core::instance()->kaduWindow()*/))->show();
}

void SmsConfigurationUiHandler::onUserClicked(Contact contact)
{
// 	if (button == 4)
// 	{
// 		UserBox *userbox = dynamic_cast<UserBox *>(item->listBox());
// 		if (userbox)
// 		{
// 			UserListElements users = userbox->selectedUsers();
//  			if (users.count() != 1)
//  				return;

 			if (!contact.mobile().isEmpty())
 				newSms(contact.display());
// 		}
// 	}
}

void SmsConfigurationUiHandler::onUserDblClicked(Chat *chat)
{
	kdebugf();
	Contact contact = chat->contacts().toContactList().at(0);
	if ((contact.accountDatas().isEmpty() || contact == Core::instance()->myself()) && !contact.mobile().isEmpty())
		newSms(contact.display());
	kdebugf2();
}

void SmsConfigurationUiHandler::registerGateway(QString name, isValidFunc* f)
{
	kdebugf();
	QStringList priority = config_file.readEntry("SMS", "Priority").split(";");
	if (!priority.contains(name))
	{
		priority += name;
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
	QStringList priorities = config_file.readEntry("SMS", "Priority").split(";");

	foreach(const QString &gate, priorities)
	{
		if (gateways.contains(gate))
		{
			isValidFunc *f = gateways[gate];
			SmsGateway *Gateway = f(number, this);
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
	int index = gatewayListWidget->currentRow();
	if (index < 1)
		return;

	QListWidgetItem * item = gatewayListWidget->takeItem(index);
	gatewayListWidget->insertItem(--index, item);
	item->setSelected(true);
	gatewayListWidget->setCurrentItem(item);
}

void SmsConfigurationUiHandler::onDownButton()
{
	int index = gatewayListWidget->currentRow();
	if (index == gatewayListWidget->count() - 1 || index == -1)
		return;

	QListWidgetItem *item = gatewayListWidget->takeItem(index);
	gatewayListWidget->insertItem(++index, item);
	item->setSelected(true);
	gatewayListWidget->setCurrentItem(item);
}

void SmsConfigurationUiHandler::sendSmsActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (window)
	{
		ContactList users = window->contacts().toContactList();

		if (users.count() == 1 && !users[0].mobile().isEmpty())
		{
			newSms(users[0].display());
			return;
		}
	}
	newSms(QString::null);

	kdebugf2();
}

void SmsConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(destroyed(QObject *)), this, SLOT(mainConfigurationWindowDestroyed()));

	useBuiltIn = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widget()->widgetById("sms/useBuildInApp"));
	customApp = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("sms/customApp"));
	useCustomString = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widget()->widgetById("sms/useCustomString"));
	customString = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("sms/customString"));

	ConfigGroupBox *gatewayGroupBox = mainConfigurationWindow->widget()->configGroupBox("SMS", "General", "Gateways");

	QWidget *gatewayWidget = new QWidget(gatewayGroupBox->widget());

	QHBoxLayout *gatewayLayout = new QHBoxLayout(gatewayWidget);
	gatewayLayout->setSpacing(5);

	gatewayListWidget = new QListWidget(gatewayWidget);

	QWidget *buttons = new QWidget(gatewayWidget);
	QVBoxLayout *buttonsLayout = new QVBoxLayout(buttons);
	buttonsLayout->setSpacing(5);

	QPushButton *up = new QPushButton(tr("Up"), buttons);
	QPushButton *down = new QPushButton(tr("Down"), buttons);

	buttonsLayout->addWidget(up);
	buttonsLayout->addWidget(down);
	buttonsLayout->addStretch(100);

	gatewayLayout->addWidget(gatewayListWidget);
	gatewayLayout->addWidget(buttons);

	connect(up, SIGNAL(clicked()), this, SLOT(onUpButton()));
	connect(down, SIGNAL(clicked()), this, SLOT(onDownButton()));

	gatewayGroupBox->addWidgets(new QLabel(tr("Gateways priorites"), gatewayGroupBox->widget()), gatewayWidget);
	
	connect(useBuiltIn, SIGNAL(toggled(bool)), this, SLOT(onSmsBuildInCheckToggle(bool)));
	connect(useCustomString, SIGNAL(toggled(bool)), customString, SLOT(setEnabled(bool)));

// TODO: fix it, should be ':' not ';'
	QStringList priority = config_file.readEntry("SMS", "Priority").split(";");

	foreach(const QString &gate, priority)
		if (gateways.contains(gate))
			gatewayListWidget->addItem(gate);

	foreach(const QString &key, gateways.keys())
		if (gatewayListWidget->findItems(key, 0).isEmpty())
			gatewayListWidget->addItem(key);
}

void SmsConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
	gatewayListWidget = 0; // protect configurationUpdated
}

void SmsConfigurationUiHandler::createDefaultConfiguration()
{
	config_file.addVariable("SMS", "Priority", QString::null);
	config_file.addVariable("SMS", "BuiltInApp", true);
	config_file.addVariable("SMS", "SmsNick", "");
	config_file.addVariable("SMS", "UseCustomString", false);

	config_file.addVariable("ShortCuts", "kadu_sendsms", "Ctrl+S");
}

SmsConfigurationUiHandler *smsConfigurationUiHandler;

/** @} */

