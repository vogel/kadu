/***************************************************************************
                          register.cpp  -  description
                             -------------------
    begin                : Sat Dec 8 2001
    copyright            : (C) 2001 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

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

Register::Register(QDialog *parent, const char *name) : QDialog (parent, name) {
	QGridLayout *grid = new QGridLayout(this, 5, 2, 6, 5);

	QLabel *l_pwd = new QLabel(this);
	l_pwd->setText(i18n("Password"));

	QLabel *l_pwd2 = new QLabel(this);
	l_pwd2->setText(i18n("Retype password"));

	QLabel *l_mail = new QLabel(this);
	l_mail->setText(i18n("E-mail"));

	QPushButton *snd = new QPushButton(this);
	snd->setText(i18n("Register"));
	QObject::connect(snd, SIGNAL(clicked()), this, SLOT(doRegister()));

	pwd = new QLineEdit(this);
	pwd->setEchoMode(QLineEdit::Password);
	pwd2 = new QLineEdit(this);
	pwd2->setEchoMode(QLineEdit::Password);
	mail = new QLineEdit(this);

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
}

void Register::doRegister() {
	if (strcmp(pwd->text().latin1(),pwd2->text().latin1()) != 0) {
		QMessageBox::warning( this, "Kadu",i18n("Passwords do not match"),"OK", 0, 0, 1);
		return;
		}

	if (strcmp(pwd->text().latin1(),"") == 0 || strcmp(mail->text().latin1(),"") == 0) {
		QMessageBox::warning( this, "Kadu",i18n("Please fill out all fields"),"OK", 0, 0, 1);
		return;
		}

	char *email = strdup(mail->text().latin1());
	char *password = strdup(pwd->text().latin1());

	if (!(h = gg_register(email, password, 1))) {
		status->setText(i18n("Error"));
		return;
		}

	sokiet = new QTimer;
	QObject::connect(sokiet, SIGNAL(timeout()), this, SLOT(watchSocket()));		
	sokiet->start(50);
	status->setText(i18n("Registering"));
}

void Register::watchSocket() {
	fd_set rd, wr, ex;

	FD_ZERO(&rd);
	FD_ZERO(&wr);
	FD_ZERO(&ex);

	if ((h->check & GG_CHECK_READ))
		FD_SET(h->fd, &rd);
	if ((h->check & GG_CHECK_WRITE))
        	FD_SET(h->fd, &wr);
	FD_SET(h->fd, &ex);

	if (select(h->fd + 1, &rd, &wr, &ex, NULL) == -1 || FD_ISSET(h->fd, &ex)) {
		if (errno == EINTR)
			return;
		gg_free_register(h);
		perror("select");
		status->setText(i18n("Error"));
		sokiet->stop();
		return;
		}

	if (FD_ISSET(h->fd, &rd) || FD_ISSET(h->fd, &wr)) {
		if (gg_register_watch_fd(h) == -1) {
			gg_free_register(h);
			fprintf(stderr, "KK error registering\n");
			status->setText(i18n("Error"));
			sokiet->stop();
			return;
			}
		if (h->state == GG_STATE_ERROR) {
			gg_free_register(h);
			fprintf(stderr, "KK error registering\n");
			status->setText(i18n("Error"));
			sokiet->stop();
			return;
			}
		if (h->state == GG_STATE_DONE) {													
			p = (struct gg_pubdir *) h->data;
			fprintf(stderr, "KK register success=%d, uin=%ld\n", p->success, p->uin);
			status->setText(i18n("Success!"));
			sokiet->stop();											
			uin = p->uin;
			gg_free_register(h);
			QMessageBox::information( 0, "Kadu",i18n("Registration was successful. Your new number is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(uin),"OK", 0, 0, 1);
			ask();
			Adduser *au;
			au = new Adduser(0,"add_user");
			au->show();
			close(true);
			}
		}
}

void createKonfig(char * password, uin_t uin) {
	FILE *f;
	char *home = getenv("HOME");
	struct passwd *pw;

	if (!home) {
		if (!(pw = getpwuid(getuid())))
			return;
		home = pw->pw_dir;
		}

	struct stat *buf;	
	char path2[1023];
	snprintf(path2, sizeof(path2), "%s/.gg", home);	
	stat(path2, buf);
	if (S_ISDIR(buf->st_mode))
		fprintf(stderr, "KK createKonfig(): Directory %s exists\n", path2);
	else {
		fprintf(stderr, "KK createKonfig(): Creating directory\n");
		if (mkdir(path2, 0700) != 0 ) {
			perror("mkdir");
			return;
			}
		}

	char path3[1023];
	snprintf(path3, sizeof(path3), "%s/config", path2);

	fprintf(stderr,"KK createKonfig(): Writing config files...\n");
	KConfig *konf;
	konf = new KConfig(path3);
	konf->setGroup("Global");
	konf->writeEntry("UIN",config.uin);
	konf->writeEntry("Password",config.password);
	konf->sync();

	fprintf(stderr,"KK createKonfig(): Config file created\n");
}

void Register::ask() {
	if (updateconfig->isChecked()) {
//  	printf("is checked\n");
//		createKonfig();
		config.uin = uin;
		config.password = strdup(pwd->text().latin1());
		kadu->setCaption(i18n("Kadu: new user"));
		createKonfig(config.password, config.uin);
		}
}

#include "register.moc"
