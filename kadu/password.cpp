#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>

#include "misc.h"
#include "kadu.h"
#include "config_file.h"
#include "config_dialog.h"
#include "debug.h"
#include "password.h"

remindPassword::remindPassword() {
	kdebug("remindPassword::remindPassword()\n");
	snr = snw = NULL;
	h = NULL;
}

remindPassword::~remindPassword() {
	kdebug("remindPassword::~remindPassword()\n");
	
	deleteSocketNotifiers();
	if (h) {
		gg_remind_passwd_free(h);
		h = NULL;
		}
}

void remindPassword::start() {
	kdebug("remindPassword::start()\n");
	if (!(h = gg_remind_passwd(config_file.readNumEntry("Global","UIN"), 1))) {
		showErrorMessageBox();
		deleteLater();
		return;
		}

	createSocketNotifiers();
}

void remindPassword::showErrorMessageBox() {
	kdebug("remindPassword::showErrorMessageBox()\n");
	QMessageBox::information(0, i18n("Remind password"),
		i18n("Error during remind password"), i18n("OK"), 0, 0, 1);
}

void remindPassword::createSocketNotifiers() {
	kdebug("remindPassword::createSocketNotifiers()\n");

	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read, kadu);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));
	
	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write, kadu);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void remindPassword::deleteSocketNotifiers() {
	kdebug("remindPassword::deleteSocketNotifiers()\n");
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

void remindPassword::dataReceived() {
	kdebug("remindPassword::dataReceived()\n");
	if (h->check & GG_CHECK_READ)
		socketEvent();
}

void remindPassword::dataSent() {
	kdebug("remindPassword::dataSent()\n");
	snw->setEnabled(false);
	if (h->check & GG_CHECK_WRITE)
		socketEvent();
}

void remindPassword::socketEvent() {
	kdebug("remindPassword::socketEvent()\n");
	if (gg_remind_passwd_watch_fd(h) == -1) {
		deleteSocketNotifiers();
		kdebug("remindPassword::socketEvent(): error reminding password!\n");
		showErrorMessageBox();
		deleteLater();
		return;
		}
	struct gg_pubdir *p = (struct gg_pubdir *)h->data;
	switch (h->state) {
		case GG_STATE_CONNECTING:
			kdebug("remindPassword::socketEvent(): changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
			break;
		case GG_STATE_ERROR:
			kdebug("remindPassword::socketEvent(): error reminding password!\n");
			deleteSocketNotifiers();
			showErrorMessageBox();
			deleteLater();
			break;
		case GG_STATE_DONE:
			kdebug("remindPassword::socketEvent(): success!\n");
			deleteSocketNotifiers();
			if (!p->success) {
				kdebug("remindPassword::socketEvent(): error reminding password!\n");
				showErrorMessageBox();
				}
			deleteLater();
			break;
		default:
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
		}
}

changePassword::changePassword(QDialog *parent, const char *name) : QDialog(parent, name, FALSE, Qt::WDestructiveClose) {
	kdebug("changePassword::changePassword()\n");
	snr = snw = NULL;
	h = NULL;

	QGridLayout *grid = new QGridLayout(this, 7, 2, 6, 5);
	
	QLabel *l_actpwd = new QLabel(i18n("Actual password"),this);
	actpwd = new QLineEdit(this);
	actpwd->setEchoMode(QLineEdit::Password);

	QLabel *l_actemail = new QLabel(i18n("Actual email"),this);
	actemail = new QLineEdit(this);

	QLabel *l_newpwd = new QLabel(i18n("New password"),this);
	newpwd = new QLineEdit(this);
	newpwd->setEchoMode(QLineEdit::Password);
	
	QLabel *l_newpwd2 = new QLabel(i18n("Retype new password"),this);
	newpwd2 = new QLineEdit(this);
	newpwd2->setEchoMode(QLineEdit::Password);

	QLabel *l_newemail = new QLabel(i18n("New email"),this);
	newemail = new QLineEdit(this);

	status = new QLabel(this);

	QPushButton *okbtn = new QPushButton(this);
	okbtn->setText(i18n("OK"));
	QObject::connect(okbtn, SIGNAL(clicked()), this, SLOT(start()));

	grid->addWidget(l_actpwd, 0, 0);
	grid->addWidget(actpwd, 0, 1);
	grid->addWidget(l_actemail, 1, 0);
	grid->addWidget(actemail, 1, 1);
	grid->addWidget(l_newpwd, 2, 0);
	grid->addWidget(newpwd, 2, 1);
	grid->addWidget(l_newpwd2, 3, 0);
	grid->addWidget(newpwd2, 3, 1);
	grid->addWidget(l_newemail, 4, 0);
	grid->addWidget(newemail, 4, 1);
	grid->addWidget(status, 6, 0);
	grid->addWidget(okbtn, 6, 1);
	grid->addRowSpacing(3, 20);

	setCaption(i18n("Change password"));
	resize(300, 180);
}

void changePassword::closeEvent(QCloseEvent *e) {
	kdebug("changePassword::closeEvent()\n");
	
	deleteSocketNotifiers();
	if (h) {
		gg_change_passwd_free(h);
		h = NULL;
		}
	QDialog::closeEvent(e);
}

void changePassword::start() {
	kdebug("changePassword::start()\n");
	if (!actpwd->text().length() || !actemail->text().length() || !newpwd->text().length() ||
		newpwd->text() != newpwd2->text() || !newemail->text().length()) {
		status->setText(i18n("Bad data"));
		return;
		}
	char *actpasswd, *newpasswd, *actmail, *newmail;
	actpasswd = strdup(unicode2cp(actpwd->text()).data());
	newpasswd = strdup(unicode2cp(newpwd->text()).data());
	actmail = strdup(unicode2cp(actemail->text()).data());
	newmail = strdup(unicode2cp(newemail->text()).data());
	if (!(h = gg_change_passwd2(config_file.readNumEntry("Global","UIN"), actpasswd, newpasswd, actmail, newmail, 1))) {
		status->setText(i18n("Error"));
		free(actpasswd);
		free(newpasswd);
		free(actmail);
		free(newmail);
		return;
		}
	free(actpasswd);
	free(newpasswd);
	free(actmail);
	free(newmail);
	setEnabled(false);
	createSocketNotifiers();
}

void changePassword::createSocketNotifiers() {
	kdebug("changePassword::createSocketNotifiers()\n");

	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read, kadu);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));
	
	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write, kadu);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void changePassword::deleteSocketNotifiers() {
	kdebug("changePassword::deleteSocketNotifiers()\n");
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

void changePassword::dataReceived() {
	kdebug("changePassword::dataReceived()\n");
	if (h->check & GG_CHECK_READ)
		socketEvent();
}

void changePassword::dataSent() {
	kdebug("changePassword::dataSent()\n");
	snw->setEnabled(false);
	if (h->check & GG_CHECK_WRITE)
		socketEvent();
}

void changePassword::socketEvent() {
	kdebug("changePassword::socketEvent()\n");
	if (gg_remind_passwd_watch_fd(h) == -1) {
		deleteSocketNotifiers();
		gg_change_passwd_free(h);
		h = NULL;
		kdebug("changePassword::socketEvent(): error changing password!\n");
		status->setText(i18n("Error"));
		setEnabled(true);
		return;
		}
	struct gg_pubdir *p = (struct gg_pubdir *)h->data;
	switch (h->state) {
		case GG_STATE_CONNECTING:
			kdebug("changePassword::socketEvent(): changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
			break;
		case GG_STATE_ERROR:
			kdebug("changePassword::socketEvent(): error changing password!\n");
			deleteSocketNotifiers();
			gg_change_passwd_free(h);
			h = NULL;
			status->setText(i18n("Error"));
			setEnabled(true);
			break;
		case GG_STATE_DONE:
			kdebug("changePassword::socketEvent(): success!\n");
			deleteSocketNotifiers();
			if (!p->success) {
				gg_change_passwd_free(h);
				h = NULL;
				kdebug("changePassword::socketEvent(): error changing password!\n");
				status->setText(i18n("Error"));
				setEnabled(true);
				}
			else {
				saveKaduConfig();
				accept();
				}
			break;
		default:
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
		}
}

