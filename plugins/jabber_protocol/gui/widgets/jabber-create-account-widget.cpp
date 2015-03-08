/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCrypto/QtCrypto>
#include <QtGui/QIntValidator>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

#include "accounts/account-manager.h"
#include "gui/widgets/simple-configuration-value-state-notifier.h"
#include "gui/windows/jabber-wait-for-account-register-window.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "identities/identity-manager.h"
#include "protocols/protocols-manager.h"
#include "services/jabber-error-service.h"
#include "services/jabber-register-account-service.h"
#include "services/jabber-register-account.h"
#include "jabber-account-details.h"
#include "jabber-protocol-factory.h"

#include "jabber-create-account-widget.h"

JabberCreateAccountWidget::JabberCreateAccountWidget(bool showButtons, QWidget *parent) :
		AccountCreateWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)), this, SLOT(dataChanged()));

	createGui(showButtons);
	resetGui();
}

JabberCreateAccountWidget::~JabberCreateAccountWidget()
{
}

void JabberCreateAccountWidget::createGui(bool showButtons)
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	QWidget *jidWidget = new QWidget(this);
	QGridLayout *jidLayout = new QGridLayout(jidWidget);
	jidLayout->setSpacing(0);
	jidLayout->setMargin(0);
	jidLayout->setColumnStretch(0, 2);
	jidLayout->setColumnStretch(2, 2);

	Username = new QLineEdit(this);
	connect(Username, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	jidLayout->addWidget(Username);

	QLabel *atLabel = new QLabel("@", this);
	jidLayout->addWidget(atLabel, 0, 1);

	Domain = new QComboBox();
	Domain->setEditable(true);
	connect(Domain, SIGNAL(currentIndexChanged(QString)), this, SLOT(dataChanged()));
	connect(Domain, SIGNAL(editTextChanged(QString)), this, SLOT(dataChanged()));
	jidLayout->addWidget(Domain, 0, 2);

	layout->addRow(tr("Username") + ':', jidWidget);

	NewPassword = new QLineEdit(this);
	connect(NewPassword, SIGNAL(textEdited(const QString &)), this, SLOT(dataChanged()));
	NewPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Password") + ':', NewPassword);

	ReNewPassword = new QLineEdit(this);
	connect(ReNewPassword, SIGNAL(textEdited(const QString &)), this, SLOT(dataChanged()));
	ReNewPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Retype Password") + ':', ReNewPassword);

	RememberPassword = new QCheckBox(tr("Remember password"), this);
	layout->addWidget(RememberPassword);

	EMail = new QLineEdit{this};
	layout->addRow(tr("E-mail:"), EMail);

	QLabel *infoLabel = new QLabel(tr("<font size='-1'><i>Some servers require your e-mail address during registration.</i></font>"), this);
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout->addRow(0, infoLabel);

	IdentityCombo = new IdentitiesComboBox(this);
	connect(IdentityCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Identity") + ':', IdentityCombo);

	infoLabel = new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.</i></font>"), this);
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout->addRow(0, infoLabel);

	mainLayout->addStretch(100);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	RegisterAccountButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Register Account"), this);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

	buttons->addButton(RegisterAccountButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(RegisterAccountButton, SIGNAL(clicked(bool)), this, SLOT(apply()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));

	if (!showButtons)
		buttons->hide();

//	Domain->addItem("adastra.re");
//	Domain->addItem("alpha-labs.net");
	Domain->addItem("apocalysto.ru");
	Domain->addItem("bigowl.net");
	Domain->addItem("blah.im");
	Domain->addItem("cabbar.org");
//	Domain->addItem("chatme.biz");
//	Domain->addItem("chatme.community");
//	Domain->addItem("chatme.education");
//	Domain->addItem("chatme.im");
//	Domain->addItem("chatme.sexy");
//	Domain->addItem("chatme.singles");
//	Domain->addItem("chatme.social");
//	Domain->addItem("chatme.xyz");
//	Domain->addItem("coderollers.com");
	Domain->addItem("creep.im");
	Domain->addItem("crypt.am");
	Domain->addItem("crypt.mn");
//	Domain->addItem("default.rs");
//	Domain->addItem("dotchat.me");
	Domain->addItem("e-utp.net");
//	Domain->addItem("einfachjabber.de");
	Domain->addItem("foxba.se");
	Domain->addItem("freamware.net");
//	Domain->addItem("fysh.in");
//	Domain->addItem("gabbler.de");
	Domain->addItem("gajim.org");
	Domain->addItem("gnuhost.eu");
//	Domain->addItem("gojabber.org");
//	Domain->addItem("hot-chilli.eu");
//	Domain->addItem("hot-chilli.net");
//	Domain->addItem("igniterealtime.org");
//	Domain->addItem("im.hot-chilli.eu");
//	Domain->addItem("im.hot-chilli.net");
//	Domain->addItem("im.it-native.de");
//	Domain->addItem("inbox.im");
	Domain->addItem("injabber.info");
//	Domain->addItem("is-a-furry.org");
//	Domain->addItem("jabb3r.net");
	Domain->addItem("jabbeng.in");
//	Domain->addItem("jabber-br.org");
//	Domain->addItem("jabber-hosting.de");
//	Domain->addItem("jabber.at");
//	Domain->addItem("jabber.ccc.de");
	Domain->addItem("jabber.co.cm");
//	Domain->addItem("jabber.cz");
//	Domain->addItem("jabber.de");
//	Domain->addItem("jabber.fourecks.de");
//	Domain->addItem("jabber.hot-chilli.eu");
//	Domain->addItem("jabber.hot-chilli.net");
//	Domain->addItem("jabber.i-pobox.net");
//	Domain->addItem("jabber.kernel-error.de");
//	Domain->addItem("jabber.minus273.org");
	Domain->addItem("jabber.no-sense.net");
	Domain->addItem("jabber.ru.com");
	Domain->addItem("jabber.rueckgr.at");
//	Domain->addItem("jabber.se");
//	Domain->addItem("jabber.smash-net.org");
	Domain->addItem("jabber.theforest.us");
//	Domain->addItem("jabber.web.id");
//	Domain->addItem("jabber.wien");
//	Domain->addItem("jabber.zone");
	Domain->addItem("jabberafrica.org");
//	Domain->addItem("jabberforum.de");
//	Domain->addItem("jabberon.net");
//	Domain->addItem("jabberon.ru");
//	Domain->addItem("jabberwiki.de");
//	Domain->addItem("jabbim.com");
//	Domain->addItem("jabbim.cz");
//	Domain->addItem("jabbim.pl");
//	Domain->addItem("jabbim.sk");
//	Domain->addItem("jabjab.de");
	Domain->addItem("jappix.com");
	Domain->addItem("jid.su");
//	Domain->addItem("jix.im");
//	Domain->addItem("jodo.im");
	Domain->addItem("jsmart.web.id");
//	Domain->addItem("kdex.de");
//	Domain->addItem("lethyro.net");
//	Domain->addItem("limun.org");
//	Domain->addItem("linuxlovers.at");
//	Domain->addItem("lsd-25.ru");
//	Domain->addItem("mijabber.es");
//	Domain->addItem("miscastonline.com");
	Domain->addItem("neko.im");
//	Domain->addItem("njs.netlab.cz");
//	Domain->addItem("ohai.su");
	Domain->addItem("p-h.im");
//	Domain->addItem("planetjabber.de");
//	Domain->addItem("pod.so");
//	Domain->addItem("radiodd.de");
//	Domain->addItem("richim.org");
//	Domain->addItem("rosolina.estate");
//	Domain->addItem("rows.io");
//	Domain->addItem("securejabber.me");
//	Domain->addItem("see.ph");
//	Domain->addItem("slang.cool");
//	Domain->addItem("sternenschweif.de");
//	Domain->addItem("suchat.org");
//	Domain->addItem("talk.planetjabber.de");
	Domain->addItem("twattle.net");
	Domain->addItem("uplink.io");
//	Domain->addItem("webchat.domains");
	Domain->addItem("wtfismyip.com");
//	Domain->addItem("wusz.org");
//	Domain->addItem("xmpp-hosting.de");
	Domain->addItem("xmpp.cm");
//	Domain->addItem("xmpp.guru");
	Domain->addItem("xmpp.jp");
	Domain->addItem("xmpp.kz");
	Domain->addItem("xmpp.ninja");
	Domain->addItem("xmpp.pro");
//	Domain->addItem("xmpp.ru.net");
	Domain->addItem("xmpp.su");
//	Domain->addItem("xmpp.technology");
//	Domain->addItem("xmpp.tips");
//	Domain->addItem("xmpp.zone");
	Domain->addItem("xmppcomm.com");
	Domain->addItem("xmppcomm.net");
//	Domain->addItem("xmppnet.de");
}

bool JabberCreateAccountWidget::checkSSL()
{
	if (!QCA::isSupported("tls"))
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Cannot enable secure connection. SSL/TLS plugin not found."), QMessageBox::Ok, this);
		return false;
	}
	return true;
}

void JabberCreateAccountWidget::dataChanged()
{
	bool valid = !Domain->currentText().isEmpty()
			&& !Username->text().isEmpty()
			&& !NewPassword->text().isEmpty()
			&& !ReNewPassword->text().isEmpty()
			&& !AccountManager::instance()->byId("jabber", Username->text() + '@' + Domain->currentText())
			&& IdentityCombo->currentIdentity();

	RegisterAccountButton->setEnabled(valid);

	if (Domain->currentText().isEmpty()
			&& Username->text().isEmpty()
			&& NewPassword->text().isEmpty()
			&& ReNewPassword->text().isEmpty()
			&& RememberPassword->isChecked()
			&& 0 == IdentityCombo->currentIndex())
		simpleStateNotifier()->setState(StateNotChanged);
	else
		simpleStateNotifier()->setState(valid ? StateChangedDataValid : StateChangedDataInvalid);
}

void JabberCreateAccountWidget::apply()
{
	if (NewPassword->text() != ReNewPassword->text())
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Invalid data entered in required fields.\n\n"
			"Password entered in both fields (\"New password\" and \"Retype password\") "
			"must be the same!"), QMessageBox::Ok, this);
		return;
	}

	auto errorService = new JabberErrorService{this};
	auto registerAccountService = new JabberRegisterAccountService{this};
	registerAccountService->setErrorService(errorService);

	auto jid = Jid{Username->text(), Domain->currentText(), QString{}};
	auto registerAccount = registerAccountService->registerAccount(jid, NewPassword->text(), EMail->text());

	auto window = new JabberWaitForAccountRegisterWindow(registerAccount);
	connect(window, SIGNAL(jidRegistered(Jid)), this, SLOT(jidRegistered(Jid)));
	window->exec();
}

void JabberCreateAccountWidget::cancel()
{
	resetGui();
}

void JabberCreateAccountWidget::resetGui()
{
	Username->clear();
	Domain->setCurrentIndex(-1);
	NewPassword->clear();
	ReNewPassword->clear();
	RememberPassword->setChecked(true);
	IdentityManager::instance()->removeUnused();
	IdentityCombo->setCurrentIndex(0);
	RegisterAccountButton->setEnabled(false);

	simpleStateNotifier()->setState(StateNotChanged);
}

void JabberCreateAccountWidget::jidRegistered(const Jid &jid)
{
	if (jid.isEmpty())
	{
		emit accountCreated(Account());
		return;
	}

	Account jabberAccount = Account::create("jabber");
	jabberAccount.setId(jid.bare());
	jabberAccount.setHasPassword(true);
	jabberAccount.setPassword(NewPassword->text());
	jabberAccount.setRememberPassword(RememberPassword->isChecked());
	// bad code: order of calls is important here
	// we have to set identity after password
	// so in cache of identity status container it already knows password and can do status change without asking user for it
	jabberAccount.setAccountIdentity(IdentityCombo->currentIdentity());

	JabberAccountDetails *details = dynamic_cast<JabberAccountDetails *>(jabberAccount.details());
	if (details)
		details->setState(StorableObject::StateNew);

	resetGui();

	emit accountCreated(jabberAccount);
}

#include "moc_jabber-create-account-widget.cpp"
