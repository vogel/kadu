#include <klocale.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>

#include "misc.h"
#include "kadu.h"
#include "config.h"
#include "password.h"

remindPassword::remindPassword() {
	fprintf(stderr, "KK remindPassword::remindPassword()\n");
	snr = snw = NULL;
	h = NULL;
}

remindPassword::~remindPassword() {
	fprintf(stderr, "KK remindPassword::~remindPassword()\n");
	
	deleteSocketNotifiers();
	if (h) {
		gg_remind_passwd_free(h);
		h = NULL;
		}
}

void remindPassword::start() {
	fprintf(stderr, "KK remindPassword::start()\n");
	if (!(h = gg_remind_passwd(config.uin, 1))) {
		showErrorMessageBox();
		deleteLater();
		return;
		}

	createSocketNotifiers();
}

void remindPassword::showErrorMessageBox() {
	fprintf(stderr, "KK remindPassword::showErrorMessageBox()\n");
	QMessageBox::information(0, i18n("Remind password"),
		i18n("Error during remind password"), i18n("OK"), 0, 0, 1);
}

void remindPassword::createSocketNotifiers() {
	fprintf(stderr, "KK remindPassword::createSocketNotifiers()\n");

	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read, kadu);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));
	
	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write, kadu);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void remindPassword::deleteSocketNotifiers() {
	fprintf(stderr, "KK remindPassword::deleteSocketNotifiers()\n");
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
	fprintf(stderr, "KK remindPassword::dataReceived()\n");
	if (h->check & GG_CHECK_READ)
		socketEvent();
}

void remindPassword::dataSent() {
	fprintf(stderr, "KK remindPassword::dataSent()\n");
	snw->setEnabled(false);
	if (h->check & GG_CHECK_WRITE)
		socketEvent();
}

void remindPassword::socketEvent() {
	fprintf(stderr, "KK remindPassword::socketEvent()\n");
	if (gg_remind_passwd_watch_fd(h) == -1) {
		deleteSocketNotifiers();
		fprintf(stderr, "KK remindPassword::socketEvent(): error reminding password!\n");
		showErrorMessageBox();
		deleteLater();
		return;
		}
	struct gg_pubdir *p = (struct gg_pubdir *)h->data;
	switch (h->state) {
		case GG_STATE_CONNECTING:
			fprintf(stderr, "KK remindPassword::socketEvent(): changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
			break;
		case GG_STATE_ERROR:
			fprintf(stderr, "KK remindPassword::socketEvent(): error reminding password!\n");
			deleteSocketNotifiers();
			showErrorMessageBox();
			deleteLater();
			break;
		case GG_STATE_DONE:
			fprintf(stderr, "KK remindPassword::socketEvent(): success!\n");
			deleteSocketNotifiers();
			if (!p->success) {
				fprintf(stderr, "KK remindPassword::socketEvent(): error reminding password!\n");
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
	fprintf(stderr, "KK changePassword::changePassword()\n");
	snr = snw = NULL;
	h = NULL;

	QGridLayout *grid = new QGridLayout(this, 7, 2, 6, 5);
	
	QLabel *l_actpwd = new QLabel(this);
	l_actpwd->setText(i18n("Actual password"));
	actpwd = new QLineEdit(this);
	actpwd->setEchoMode(QLineEdit::Password);

	QLabel *l_actemail = new QLabel(this);
	l_actemail->setText(i18n("Actual email"));
	actemail = new QLineEdit(this);

	QLabel *l_newpwd = new QLabel(this);
	l_newpwd->setText(i18n("New password"));
	newpwd = new QLineEdit(this);
	newpwd->setEchoMode(QLineEdit::Password);
	
	QLabel *l_newpwd2 = new QLabel(this);
	l_newpwd2->setText(i18n("Retype new password"));
	newpwd2 = new QLineEdit(this);
	newpwd2->setEchoMode(QLineEdit::Password);

	QLabel *l_newemail = new QLabel(this);
	l_newemail->setText(i18n("New email"));
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
	fprintf(stderr, "KK changePassword::closeEvent()\n");
	
	deleteSocketNotifiers();
	if (h) {
		gg_change_passwd_free(h);
		h = NULL;
		}
	QDialog::closeEvent(e);
}

void changePassword::start() {
	fprintf(stderr, "KK changePassword::start()\n");
	if (!actpwd->text().length() || !actemail->text().length() || !newpwd->text().length() ||
		newpwd->text() != newpwd2->text() || !newemail->text().length()) {
		status->setText(i18n("Bad data"));
		return;
		}
	if (!(h = gg_change_passwd2(config.uin, actpwd->text().local8Bit(), newpwd->text().local8Bit(),
		actemail->text().local8Bit(), newemail->text().local8Bit(), 1))) {
		status->setText(i18n("Error"));
		return;
		}
	setEnabled(false);
	createSocketNotifiers();
}

void changePassword::createSocketNotifiers() {
	fprintf(stderr, "KK changePassword::createSocketNotifiers()\n");

	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read, kadu);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));
	
	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write, kadu);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void changePassword::deleteSocketNotifiers() {
	fprintf(stderr, "KK changePassword::deleteSocketNotifiers()\n");
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
	fprintf(stderr, "KK changePassword::dataReceived()\n");
	if (h->check & GG_CHECK_READ)
		socketEvent();
}

void changePassword::dataSent() {
	fprintf(stderr, "KK changePassword::dataSent()\n");
	snw->setEnabled(false);
	if (h->check & GG_CHECK_WRITE)
		socketEvent();
}

void changePassword::socketEvent() {
	fprintf(stderr, "KK changePassword::socketEvent()\n");
	if (gg_remind_passwd_watch_fd(h) == -1) {
		deleteSocketNotifiers();
		gg_change_passwd_free(h);
		h = NULL;
		fprintf(stderr, "KK changePassword::socketEvent(): error changing password!\n");
		status->setText(i18n("Error"));
		setEnabled(true);
		return;
		}
	struct gg_pubdir *p = (struct gg_pubdir *)h->data;
	switch (h->state) {
		case GG_STATE_CONNECTING:
			fprintf(stderr, "KK changePassword::socketEvent(): changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
			break;
		case GG_STATE_ERROR:
			fprintf(stderr, "KK changePassword::socketEvent(): error changing password!\n");
			deleteSocketNotifiers();
			gg_change_passwd_free(h);
			h = NULL;
			status->setText(i18n("Error"));
			setEnabled(true);
			break;
		case GG_STATE_DONE:
			fprintf(stderr, "KK changePassword::socketEvent(): success!\n");
			deleteSocketNotifiers();
			if (!p->success) {
				gg_change_passwd_free(h);
				h = NULL;
				fprintf(stderr, "KK changePassword::socketEvent(): error changing password!\n");
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

#include "password.moc"
