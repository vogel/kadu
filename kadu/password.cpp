#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>

#include <stdlib.h>

#include "misc.h"
#include "config_file.h"
#include "config_dialog.h"
#include "debug.h"
#include "password.h"

RemindPassword::RemindPassword() {
	kdebug("RemindPassword::RemindPassword()\n");
	snr = snw = NULL;
	h = NULL;
}

RemindPassword::~RemindPassword() {
	kdebug("RemindPassword::~RemindPassword()\n");
	
	deleteSocketNotifiers();
	if (h) {
		gg_remind_passwd_free(h);
		h = NULL;
		}
}

void RemindPassword::start() {
	kdebug("RemindPassword::start()\n");

	TokenDialog *tokendialog = new TokenDialog();
	if (tokendialog->exec() != QDialog::Accepted) {
		delete tokendialog;
		return;
		}
	QString Tokenid, Tokenval;
	tokendialog->getToken(Tokenid, Tokenval);
	delete tokendialog;

	char *tokenid, *tokenval;
	tokenid = strdup(unicode2cp(Tokenid).data());
	tokenval = strdup(unicode2cp(Tokenval).data());
	h = gg_remind_passwd2(config_file.readNumEntry("General","UIN"), tokenid, tokenval, 1);
	free(tokenid);
	free(tokenval);
	if (!h) {
		showErrorMessageBox();
		deleteLater();
		return;
		}
	createSocketNotifiers();
}

void RemindPassword::showErrorMessageBox() {
	kdebug("RemindPassword::showErrorMessageBox()\n");
	QMessageBox::information(0, tr("Remind password"),
		tr("Error during remind password"), tr("OK"), 0, 0, 1);
}

void RemindPassword::createSocketNotifiers() {
	kdebug("RemindPassword::createSocketNotifiers()\n");

	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read, qApp->mainWidget());
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));
	
	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write, qApp->mainWidget());
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void RemindPassword::deleteSocketNotifiers() {
	kdebug("RemindPassword::deleteSocketNotifiers()\n");
	if (snr) {
		snr->setEnabled(false);
		snr->deleteLater();
		snr = NULL;
		}
	if (snw) {
		snw->setEnabled(false);
		snw->deleteLater();
		snw = NULL;
		}
}

void RemindPassword::dataReceived() {
	kdebug("RemindPassword::dataReceived()\n");
	if (h->check & GG_CHECK_READ)
		socketEvent();
}

void RemindPassword::dataSent() {
	kdebug("RemindPassword::dataSent()\n");
	snw->setEnabled(false);
	if (h->check & GG_CHECK_WRITE)
		socketEvent();
}

void RemindPassword::socketEvent() {
	kdebug("RemindPassword::socketEvent()\n");
	if (gg_remind_passwd_watch_fd(h) == -1) {
		deleteSocketNotifiers();
		kdebug("RemindPassword::socketEvent(): error reminding password!\n");
		showErrorMessageBox();
		deleteLater();
		return;
		}
	struct gg_pubdir *p = (struct gg_pubdir *)h->data;
	switch (h->state) {
		case GG_STATE_CONNECTING:
			kdebug("RemindPassword::socketEvent(): changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
			break;
		case GG_STATE_ERROR:
			kdebug("RemindPassword::socketEvent(): error reminding password!\n");
			deleteSocketNotifiers();
			showErrorMessageBox();
			deleteLater();
			break;
		case GG_STATE_DONE:
			kdebug("RemindPassword::socketEvent(): success!\n");
			deleteSocketNotifiers();
			if (!p->success) {
				kdebug("RemindPassword::socketEvent(): error reminding password!\n");
				showErrorMessageBox();
				}
			deleteLater();
			break;
		default:
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
		}
}

ChangePassword::ChangePassword(QDialog *parent, const char *name) : QDialog(parent, name, FALSE, Qt::WDestructiveClose) {
	kdebug("ChangePassword::ChangePassword()\n");
	snr = snw = NULL;
	h = NULL;

	QGridLayout *grid = new QGridLayout(this, 4, 2, 6, 5);

	QLabel *l_email = new QLabel(tr("New email"),this);
	emailedit = new QLineEdit(this);

	QLabel *l_newpwd = new QLabel(tr("New password"),this);
	newpwd = new QLineEdit(this);
	newpwd->setEchoMode(QLineEdit::Password);
	
	QLabel *l_newpwd2 = new QLabel(tr("Retype new password"),this);
	newpwd2 = new QLineEdit(this);
	newpwd2->setEchoMode(QLineEdit::Password);

	status = new QLabel(this);

	QPushButton *okbtn = new QPushButton(this);
	okbtn->setText(tr("OK"));
	QObject::connect(okbtn, SIGNAL(clicked()), this, SLOT(start()));

	grid->addWidget(l_email, 0, 0);
	grid->addWidget(emailedit, 0, 1);
	grid->addWidget(l_newpwd, 1, 0);
	grid->addWidget(newpwd, 1, 1);
	grid->addWidget(l_newpwd2, 2, 0);
	grid->addWidget(newpwd2, 2, 1);
	grid->addWidget(status, 3, 0);
	grid->addWidget(okbtn, 3, 1);
	grid->addRowSpacing(3, 20);

	setCaption(tr("Change password/email"));
	resize(300, 120);
	show();
}

void ChangePassword::closeEvent(QCloseEvent *e) {
	kdebug("ChangePassword::closeEvent()\n");
	
	deleteSocketNotifiers();
	if (h) {
		gg_change_passwd_free(h);
		h = NULL;
		}
	QDialog::closeEvent(e);
}

void ChangePassword::start() {
	kdebug("ChangePassword::start()\n");
	if (newpwd->text() != newpwd2->text()) {
		status->setText(tr("Bad data"));
		return;
		}
	TokenDialog *tokendialog = new TokenDialog();
	if (tokendialog->exec() != QDialog::Accepted) {
		delete tokendialog;
		return;
		}
	QString Tokenid, Tokenval;
	tokendialog->getToken(Tokenid, Tokenval);
	delete tokendialog;

	char *passwd, *newpasswd, *mail, *tokenid, *tokenval;
	passwd = strdup(unicode2cp(pwHash(config_file.readEntry("General", "Password"))).data());
	if (newpwd->text().length())
		newpasswd = strdup(unicode2cp(newpwd->text()).data());
	else
		newpasswd = strdup(passwd);	
	mail = strdup(unicode2cp(emailedit->text()).data());
	tokenid = strdup(unicode2cp(Tokenid).data());
	tokenval = strdup(unicode2cp(Tokenval).data());
	if (!(h = gg_change_passwd4(config_file.readNumEntry("General","UIN"), mail, passwd,
		newpasswd, tokenid, tokenval, 1))) {
		status->setText(tr("Error"));
		free(passwd);
		free(newpasswd);
		free(mail);
		free(tokenid);
		free(tokenval);
		return;
		}
	free(passwd);
	free(newpasswd);
	free(mail);
	free(tokenid);
	free(tokenval);
	setEnabled(false);
	createSocketNotifiers();
}

void ChangePassword::createSocketNotifiers() {
	kdebug("ChangePassword::createSocketNotifiers()\n");

	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read, qApp->mainWidget());
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));
	
	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write, qApp->mainWidget());
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void ChangePassword::deleteSocketNotifiers() {
	kdebug("ChangePassword::deleteSocketNotifiers()\n");
	if (snr) {
		snr->setEnabled(false);
		snr->deleteLater();
		snr = NULL;
		}
	if (snw) {
		snw->setEnabled(false);
		snw->deleteLater();
		snw = NULL;
		}
}

void ChangePassword::dataReceived() {
	kdebug("ChangePassword::dataReceived()\n");
	if (h->check & GG_CHECK_READ)
		socketEvent();
}

void ChangePassword::dataSent() {
	kdebug("ChangePassword::dataSent()\n");
	snw->setEnabled(false);
	if (h->check & GG_CHECK_WRITE)
		socketEvent();
}

void ChangePassword::socketEvent() {
	kdebug("ChangePassword::socketEvent()\n");
	if (gg_remind_passwd_watch_fd(h) == -1) {
		deleteSocketNotifiers();
		gg_change_passwd_free(h);
		h = NULL;
		kdebug("ChangePassword::socketEvent(): error changing password!\n");
		status->setText(tr("Error"));
		setEnabled(true);
		return;
		}
	struct gg_pubdir *p = (struct gg_pubdir *)h->data;
	switch (h->state) {
		case GG_STATE_CONNECTING:
			kdebug("ChangePassword::socketEvent(): changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
			break;
		case GG_STATE_ERROR:
			kdebug("ChangePassword::socketEvent(): error changing password!\n");
			deleteSocketNotifiers();
			gg_change_passwd_free(h);
			h = NULL;
			status->setText(tr("Error"));
			setEnabled(true);
			break;
		case GG_STATE_DONE:
			kdebug("ChangePassword::socketEvent(): success!\n");
			deleteSocketNotifiers();
			if (!p->success) {
				gg_change_passwd_free(h);
				h = NULL;
				kdebug("ChangePassword::socketEvent(): error changing password!\n");
				status->setText(tr("Error"));
				setEnabled(true);
				}
			else {
				config_file.writeEntry("General", "Password", pwHash(newpwd->text()));
				config_file.sync();
				accept();
				}
			break;
		default:
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
		}
}

