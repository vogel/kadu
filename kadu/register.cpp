/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlayout.h>
#include <klocale.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <errno.h>
#include <kconfig.h>
#include <stdlib.h>

// to mam nadzieje kiedys usuniemy
#include "kadu.h"
#include "adduser.h"
#include "register.h"
//

Register::Register(QDialog *parent, const char *name) : QDialog (parent, name, FALSE, Qt::WDestructiveClose) {
	fprintf(stderr, "KK Register::Register()\n");

	QGridLayout *grid = new QGridLayout(this, 5, 2, 6, 5);

	QLabel *l_pwd = new QLabel(this);
	l_pwd->setText(i18n("Password"));
	pwd = new QLineEdit(this);
	pwd->setEchoMode(QLineEdit::Password);

	QLabel *l_pwd2 = new QLabel(this);
	l_pwd2->setText(i18n("Retype password"));
	pwd2 = new QLineEdit(this);
	pwd2->setEchoMode(QLineEdit::Password);

	QLabel *l_mail = new QLabel(this);
	l_mail->setText(i18n("E-mail"));
	mail = new QLineEdit(this);

	QPushButton *snd = new QPushButton(this);
	snd->setText(i18n("Register"));
	QObject::connect(snd, SIGNAL(clicked()), this, SLOT(doRegister()));

	status = new QLabel(this);

	updateconfig = new QCheckBox(this);
	QLabel *l_updateconfig = new QLabel(this);
	l_updateconfig->setText(i18n("Create config file"));
	updateconfig->setChecked(true);
	QToolTip::add(l_updateconfig, i18n("Write the newly obtained UIN and password into a clean configuration file\nThis will erase your current config file contents if you have one"));

	grid->addWidget(l_pwd, 0, 0);
	grid->addWidget(pwd, 0, 1);
	grid->addWidget(l_pwd2, 1, 0);
	grid->addWidget(pwd2, 1, 1);
	grid->addWidget(l_mail, 2, 0);
	grid->addWidget(mail, 2, 1);
	grid->addWidget(updateconfig, 3, 0, Qt::AlignRight);
	grid->addWidget(l_updateconfig, 3, 1);
	grid->addWidget(status, 4, 0);
	grid->addWidget(snd, 4, 1);
	grid->addRowSpacing(3, 20);

	setCaption(i18n("Register user"));
	resize(240, 150);

	snr = snw = NULL;
	h = NULL;
}

void Register::doRegister() {
	fprintf(stderr, "KK Register::doRegister()\n");
	if (pwd->text() != pwd2->text()) {
		QMessageBox::warning(this, "Kadu", i18n("Passwords do not match"), i18n("OK"), 0, 0, 1);
		return;
		}

	if (!pwd->text().length()) {
		QMessageBox::warning(this, "Kadu", i18n("Please fill out all fields"), i18n("OK"), 0, 0, 1);
		return;
		}

	if (!(h = gg_register(mail->text().local8Bit(), pwd->text().local8Bit(), 1))) {
		status->setText(i18n("Error"));
		return;
		}

	status->setText(i18n("Registering"));

	setEnabled(false);
	createSocketNotifiers();
}

void Register::closeEvent(QCloseEvent *e) {
	fprintf(stderr, "KK Register::closeEvent()\n");
	deleteSocketNotifiers();
	if (h) {
		gg_free_register(h);
		h = NULL;
		}
	QDialog::closeEvent(e);
	fprintf(stderr, "KK Register::closeEvent(): end\n");
}

void Register::createSocketNotifiers() {
	fprintf(stderr, "KK Register::createSocketNotifiers()\n");

	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read, kadu);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write, kadu);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void Register::deleteSocketNotifiers() {
	fprintf(stderr, "KK Register::deleteSocketNotifiers()\n");
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

void Register::dataReceived() {
	fprintf(stderr, "KK Register::dataReceived()\n");
	if (h->check && GG_CHECK_READ)
		socketEvent();
}

void Register::dataSent() {
	fprintf(stderr, "KK Register::dataSent()\n");
	snw->setEnabled(false);
	if (h->check && GG_CHECK_WRITE)
		socketEvent();
}

void Register::socketEvent() {
	fprintf(stderr, "KK Register::socketEvent()\n");
	if (gg_register_watch_fd(h) == -1) {
		deleteSocketNotifiers();
		gg_free_register(h);
		h = NULL;
		fprintf(stderr, "KK Register::socketEvent(): error registering\n");
		status->setText(i18n("Error"));
		setEnabled(true);
		return;
		}
	struct gg_pubdir *p = (struct gg_pubdir *)h->data;
	switch (h->state) {
		case GG_STATE_CONNECTING:
			fprintf(stderr, "KK Register::socketEvent(): changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
			break;
		case GG_STATE_ERROR:
			deleteSocketNotifiers();
			gg_free_register(h);
			h = NULL;
			fprintf(stderr, "KK Register::socketEvent(): error registering\n");
			status->setText(i18n("Error"));
			setEnabled(true);
			break;
		case GG_STATE_DONE:
			deleteSocketNotifiers();
			fprintf(stderr, "KK Register::socketEvent(): success=%d, uin=%ld\n", p->success, p->uin);
			if (p->success) {
				status->setText(i18n("Success!"));
				uin = p->uin;
				QMessageBox::information(this, "Kadu", i18n("Registration was successful. Your new number is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(uin),
					i18n("OK"), 0, 0, 1);
				ask();
				fprintf(stderr, "KK Register::socketEvent() before close()\n");
//				accept();
				close();
				}
			else {
				gg_free_register(h);
				h = NULL;
				fprintf(stderr, "KK Register::socketEvent(): error registering\n");
				status->setText(i18n("Error"));
				setEnabled(true);
				}
			break;
		default:
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
		}
}

void createConfig() {
	fprintf(stderr, "KK createConfig()\n");
	char *home = getenv("HOME");
	struct passwd *pw;

	fprintf(stderr, "KK createConfig(): $HOME=%s\n", home);
	if (!home) {
		if (!(pw = getpwuid(getuid())))
			return;
		home = pw->pw_dir;
		}

	struct stat buf;	
	QString ggpath = ggPath("");
	stat(ggpath.local8Bit(), &buf);
	if (S_ISDIR(buf.st_mode))
		fprintf(stderr, "KK createConfig(): Directory %s exists\n", (const char *)ggpath.local8Bit());
	else {
		fprintf(stderr, "KK createConfig(): Creating directory\n");
		if (mkdir(ggpath.local8Bit(), 0700) != 0 ) {
			perror("mkdir");
			return;
			}
		}

	fprintf(stderr, "KK createConfig(): Writing config files...\n");
	KConfig *konf;
	konf = new KConfig(ggPath("kadu.conf"));
	konf->setGroup("Global");
	konf->writeEntry("UIN", config.uin);
	konf->writeEntry("Password", pwHash(config.password));
	konf->sync();
	delete konf;

	kadu->setCaption(QString("Kadu: %1").arg(config.uin));

	fprintf(stderr, "KK createConfig(): Config file created\n");
}

void Register::ask() {
	fprintf(stderr, "KK Register::ask()\n");
	if (updateconfig->isChecked()) {
		config.uin = uin;
		config.password = pwd->text();
		createConfig();
		}
}

#include "register.moc"
