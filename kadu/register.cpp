/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <qfile.h>

#include "kadu.h"
#include "config_file.h"
#include "debug.h"
#include "register.h"

void createConfig() {
	kdebug("createConfig()\n");
	char *home = getenv("HOME");
	struct passwd *pw;

	kdebug("createConfig(): $HOME=%s\n", home);
	if (!home) {
		if (!(pw = getpwuid(getuid())))
			return;
		home = pw->pw_dir;
		}

	struct stat buf;	
	QString ggpath = ggPath("");
	stat(ggpath.local8Bit(), &buf);
	if (S_ISDIR(buf.st_mode))
		kdebug("createConfig(): Directory %s exists\n", (const char *)ggpath.local8Bit());
	else {
		kdebug("createConfig(): Creating directory\n");
		if (mkdir(ggpath.local8Bit(), 0700) != 0 ) {
			perror("mkdir");
			return;
			}
		}

	kdebug("createConfig(): Writing config files...\n");
//	hmm wydaje mi sie ze przy obecnym config_file nie potrzebne jest to 
//	config_file.setGroup("General");
//	config_file.writeEntry("UIN", int(config.uin));
//	config_file.writeEntry("Password", pwHash(config.password));
	config_file.sync();

	kadu->setCaption(QString("Kadu: %1").arg(config_file.readNumEntry("General","UIN")));

	kdebug("createConfig(): Config file created\n");
}

Register::Register(QDialog *parent, const char *name) : QDialog (parent, name, FALSE, Qt::WDestructiveClose) {
	kdebug("Register::Register()\n");

	QGridLayout *grid = new QGridLayout(this, 7, 2, 6, 5);

	QLabel *l_pwd = new QLabel(tr("Password"),this);
	pwd = new QLineEdit(this);
	pwd->setEchoMode(QLineEdit::Password);

	QLabel *l_pwd2 = new QLabel(tr("Retype password"), this);
	pwd2 = new QLineEdit(this);
	pwd2->setEchoMode(QLineEdit::Password);

	QLabel *l_mail = new QLabel(tr("E-mail"), this);
	mailedit = new QLineEdit(this);

	QLabel *l_tokenimage = new QLabel(tr("Read this code ..."), this);
	tokenimage = new ImageWidget(this);

	QLabel *l_token = new QLabel(tr("and type here"), this);
	tokenedit = new QLineEdit(this);

	QPushButton *snd = new QPushButton(this);
	snd->setText(tr("Register"));
	QObject::connect(snd, SIGNAL(clicked()), this, SLOT(doRegister()));

	status = new QLabel(this);

	updateconfig = new QCheckBox(this);
	QLabel *l_updateconfig = new QLabel(tr("Create config file"),this);
	updateconfig->setChecked(true);
	QToolTip::add(l_updateconfig, tr("Write the newly obtained UIN and password into a clean configuration file\nThis will erase your current config file contents if you have one"));

	grid->addWidget(l_pwd, 0, 0);
	grid->addWidget(pwd, 0, 1);
	grid->addWidget(l_pwd2, 1, 0);
	grid->addWidget(pwd2, 1, 1);
	grid->addWidget(l_mail, 2, 0);
	grid->addWidget(mailedit, 2, 1);
	grid->addWidget(l_tokenimage, 3, 0);
	grid->addWidget(tokenimage, 3, 1);
	grid->addWidget(l_token, 4, 0);
	grid->addWidget(tokenedit, 4, 1);
	grid->addWidget(updateconfig, 5, 0, Qt::AlignRight);
	grid->addWidget(l_updateconfig, 5, 1);
	grid->addWidget(status, 6, 0);
	grid->addWidget(snd, 6, 1);
	grid->addRowSpacing(3, 20);

	setCaption(tr("Register user"));
	resize(240, 200);

	snr = snw = NULL;
	h = NULL;

	doGetToken();
}

void Register::doGetToken() {
	setEnabled(false);
	status->setText(tr("Getting token"));
	connect(&token_handle, SIGNAL(gotToken(struct gg_http *)),
		this, SLOT(gotTokenReceived(struct gg_http *)));
	connect(&token_handle, SIGNAL(tokenError()),
		this, SLOT(tokenErrorReceived()));
	token_handle.getToken();
}

void Register::gotTokenReceived(struct gg_http *h) {
	kdebug("Register::gotTokenReceived()\n");
	struct gg_token *t = (struct gg_token *)h->data;
	tokenid = cp2unicode((unsigned char *)t->tokenid);

	// nie optymalizowac !!!
	QByteArray buf(h->body_size);
	for (int i = 0; i < h->body_size; i++)
		buf[i] = h->body[i];

	tokenimage->setImage(buf);
	status->setText(tr("token received"));
	setEnabled(true);
	kdebug("Register::gotTokenReceived(): finished\n");
}

void Register::tokenErrorReceived() {
	kdebug("Register::tokenErrorReceived()\n");
	status->setText(tr("Couldn't get token"));
	setEnabled(true);
}

void Register::doRegister() {
	kdebug("Register::doRegister()\n");
	if (pwd->text() != pwd2->text()) {
		QMessageBox::warning(this, "Kadu", tr("Passwords do not match"), tr("OK"), 0, 0, 1);
		return;
		}

	if (!pwd->text().length()) {
		QMessageBox::warning(this, "Kadu", tr("Please fill out all fields"), tr("OK"), 0, 0, 1);
		return;
		}

	char *passwd, *token_id, *token_value, *mail;
	passwd = strdup(unicode2cp(pwd->text()).data());
	mail = strdup(unicode2cp(mailedit->text()).data());
	token_id = strdup(unicode2cp(tokenid).data());
	token_value = strdup(unicode2cp(tokenedit->text()).data());
	h = gg_register3(mail, passwd, token_id, token_value, 1);
	free(passwd);
	free(mail);
	free(token_id);
	free(token_value);
	if (!h) {
		status->setText(tr("Error"));
		return;
		}

	status->setText(tr("Registering"));

	setEnabled(false);
	createSocketNotifiers();
}

void Register::closeEvent(QCloseEvent *e) {
	kdebug("Register::closeEvent()\n");
	deleteSocketNotifiers();
	if (h) {
		gg_free_register(h);
		h = NULL;
		}
	QDialog::closeEvent(e);
	kdebug("Register::closeEvent(): end\n");
}

void Register::createSocketNotifiers() {
	kdebug("Register::createSocketNotifiers()\n");

	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read, kadu);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write, kadu);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void Register::deleteSocketNotifiers() {
	kdebug("Register::deleteSocketNotifiers()\n");
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
	kdebug("Register::dataReceived()\n");
	if (h->check && GG_CHECK_READ)
		socketEvent();
}

void Register::dataSent() {
	kdebug("Register::dataSent()\n");
	snw->setEnabled(false);
	if (h->check && GG_CHECK_WRITE)
		socketEvent();
}

void Register::socketEvent() {
	kdebug("Register::socketEvent()\n");
	if (gg_register_watch_fd(h) == -1) {
		deleteSocketNotifiers();
		gg_free_register(h);
		h = NULL;
		kdebug("Register::socketEvent(): error registering\n");
		status->setText(tr("Error"));
		setEnabled(true);
		return;
		}
	struct gg_pubdir *p = (struct gg_pubdir *)h->data;
	switch (h->state) {
		case GG_STATE_CONNECTING:
			kdebug("Register::socketEvent(): changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
			break;
		case GG_STATE_ERROR:
			deleteSocketNotifiers();
			gg_free_register(h);
			h = NULL;
			kdebug("Register::socketEvent(): error registering\n");
			status->setText(tr("Error"));
			setEnabled(true);
			break;
		case GG_STATE_DONE:
			deleteSocketNotifiers();
			kdebug("Register::socketEvent(): success=%d, uin=%ld\n", p->success, p->uin);
			if (p->success) {
				status->setText(tr("Success!"));
				uin = p->uin;
				QMessageBox::information(this, "Kadu", tr("Registration was successful. Your new number is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(uin),
					tr("OK"), 0, 0, 1);
				ask();
				gg_free_register(h);
				h = NULL;
				kdebug("Register::socketEvent(): before close()\n");
				close();
				}
			else {
				gg_free_register(h);
				h = NULL;
				kdebug("Register::socketEvent(): error registering\n");
				status->setText(tr("Error"));
				setEnabled(true);
				}
			break;
		default:
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
		}
}

void Register::ask() {
	kdebug("Register::ask()\n");
	if (updateconfig->isChecked()) {
		config_file.writeEntry("General","UIN",(int)uin);
		config_file.writeEntry("General","Password",pwHash(pwd->text()));
		createConfig();
		}
}

Unregister::Unregister(QDialog *parent, const char *name) : QDialog (parent, name, FALSE, Qt::WDestructiveClose) {
	kdebug("Unregister::Unregister()\n");

	QGridLayout *grid = new QGridLayout(this, 5, 2, 6, 5);

	QLabel *l_uin = new QLabel(tr("UIN"),this);
	uin = new QLineEdit(this);

	QLabel *l_pwd = new QLabel(tr("Password"),this);
	pwd = new QLineEdit(this);
	pwd->setEchoMode(QLineEdit::Password);

	QLabel *l_tokenimage = new QLabel(tr("Read this code ..."), this);
	tokenimage = new ImageWidget(this);

	QLabel *l_token = new QLabel(tr("and type here"), this);
	tokenedit = new QLineEdit(this);

	QPushButton *snd = new QPushButton(this);
	snd->setText(tr("Unregister"));
	QObject::connect(snd, SIGNAL(clicked()), this, SLOT(doUnregister()));

	status = new QLabel(this);

	grid->addWidget(l_uin, 0, 0);
	grid->addWidget(uin, 0, 1);
	grid->addWidget(l_pwd, 1, 0);
	grid->addWidget(pwd, 1, 1);
	grid->addWidget(l_tokenimage, 2, 0);
	grid->addWidget(tokenimage, 2, 1);
	grid->addWidget(l_token, 3, 0);
	grid->addWidget(tokenedit, 3, 1);
	grid->addWidget(status, 4, 0);
	grid->addWidget(snd, 4, 1);
	grid->addRowSpacing(3, 20);

	setCaption(tr("Unregister user"));
	resize(240, 150);

	snr = snw = NULL;
	h = NULL;

	doGetToken();
}

void Unregister::doGetToken() {
	setEnabled(false);
	status->setText(tr("Getting token"));
	connect(&token_handle, SIGNAL(gotToken(struct gg_http *)),
		this, SLOT(gotTokenReceived(struct gg_http *)));
	connect(&token_handle, SIGNAL(tokenError()),
		this, SLOT(tokenErrorReceived()));
	token_handle.getToken();
}

void Unregister::gotTokenReceived(struct gg_http *h) {
	kdebug("Unregister::gotTokenReceived()\n");
	struct gg_token *t = (struct gg_token *)h->data;
	tokenid = cp2unicode((unsigned char *)t->tokenid);

	// nie optymalizowac !!!
	QByteArray buf(h->body_size);
	for (int i = 0; i < h->body_size; i++)
		buf[i] = h->body[i];

	tokenimage->setImage(buf);
	status->setText(tr("token received"));
	setEnabled(true);
	kdebug("Unregister::gotTokenReceived(): finished\n");
}

void Unregister::tokenErrorReceived() {
	kdebug("Unregister::tokenErrorReceived()\n");
	status->setText(tr("Couldn't get token"));
	setEnabled(true);
}

void Unregister::doUnregister() {
	kdebug("Unregister::doUnregister()\n");

	if (!uin->text().toUInt() || !pwd->text().length()) {
		QMessageBox::warning(this, "Kadu", tr("Please fill out all fields"), tr("OK"), 0, 0, 1);
		return;
		}

	char *passwd, *token_id, *token_val;
	passwd = strdup(unicode2cp(pwd->text()).data());
	token_id = strdup(unicode2cp(tokenid).data());
	token_val = strdup(unicode2cp(tokenedit->text()).data());
	h = gg_unregister3(uin->text().toUInt(), passwd, token_id, token_val, 1);
	free(passwd);
	free(token_id);
	free(token_val);
	if (!h) {
		status->setText(tr("Error"));
		return;
		}

	status->setText(tr("Unregistering"));

	setEnabled(false);
	createSocketNotifiers();
}

void Unregister::closeEvent(QCloseEvent *e) {
	kdebug("Unregister::closeEvent()\n");
	deleteSocketNotifiers();
	if (h) {
		gg_free_register(h);
		h = NULL;
		}
	QDialog::closeEvent(e);
	kdebug("Unregister::closeEvent(): end\n");
}

void Unregister::createSocketNotifiers() {
	kdebug("Unregister::createSocketNotifiers()\n");

	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read, kadu);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write, kadu);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void Unregister::deleteSocketNotifiers() {
	kdebug("Unregister::deleteSocketNotifiers()\n");
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

void Unregister::dataReceived() {
	kdebug("Unregister::dataReceived()\n");
	if (h->check && GG_CHECK_READ)
		socketEvent();
}

void Unregister::dataSent() {
	kdebug("Unregister::dataSent()\n");
	snw->setEnabled(false);
	if (h->check && GG_CHECK_WRITE)
		socketEvent();
}

void Unregister::socketEvent() {
	kdebug("Unregister::socketEvent()\n");
	if (gg_register_watch_fd(h) == -1) {
		deleteSocketNotifiers();
		gg_free_register(h);
		h = NULL;
		kdebug("Unregister::socketEvent(): error unregistering\n");
		status->setText(tr("Error"));
		setEnabled(true);
		return;
		}
	struct gg_pubdir *p = (struct gg_pubdir *)h->data;
	switch (h->state) {
		case GG_STATE_CONNECTING:
			kdebug("Unregister::socketEvent(): changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
			break;
		case GG_STATE_ERROR:
			deleteSocketNotifiers();
			gg_free_register(h);
			h = NULL;
			kdebug("Unregister::socketEvent(): error unregistering\n");
			status->setText(tr("Error"));
			setEnabled(true);
			break;
		case GG_STATE_DONE:
			deleteSocketNotifiers();
			kdebug("Unregister::socketEvent(): success\n");
			if (p->success) {
				gg_free_register(h);
				h = NULL;
				status->setText(tr("Success!"));
				deleteConfig();
				kdebug("Unregister::socketEvent(): before close()\n");
				close();
				}
			else {
				gg_free_register(h);
				h = NULL;
				kdebug("Unregister::socketEvent(): error unregistering\n");
				status->setText(tr("Error"));
				setEnabled(true);
				}
			break;
		default:
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
		}
}

void Unregister::deleteConfig() {
	kdebug("Unregister::deleteConfig()\n");

	kdebug("Unregister::deleteConfig(): Deleting config file...\n");
	QFile::remove(ggPath("kadu.conf"));
	config_file.writeEntry("General","UIN",0);

	kadu->setCaption(tr("No user"));

	kdebug("Unregister::deleteConfig(): Config file deleted\n");
}

