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
#include <qfile.h>
#include <qapplication.h>
#include <qtooltip.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <stdlib.h>

#include "misc.h"
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

	qApp->mainWidget()->setCaption(QString("Kadu: %1").arg((UinType)config_file.readNumEntry("General","UIN")));

	kdebug("createConfig(): Config file created\n");
}

Register::Register(QDialog *parent, const char *name) : QDialog (parent, name, FALSE, Qt::WDestructiveClose) {
	kdebug("Register::Register()\n");

	QGridLayout *grid = new QGridLayout(this, 5, 2, 6, 5);

	QLabel *l_pwd = new QLabel(tr("Password"),this);
	pwd = new QLineEdit(this);
	pwd->setEchoMode(QLineEdit::Password);

	QLabel *l_pwd2 = new QLabel(tr("Retype password"), this);
	pwd2 = new QLineEdit(this);
	pwd2->setEchoMode(QLineEdit::Password);

	QLabel *l_mail = new QLabel(tr("E-mail"), this);
	mailedit = new QLineEdit(this);

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
	grid->addWidget(updateconfig, 3, 0, Qt::AlignRight);
	grid->addWidget(l_updateconfig, 3, 1);
	grid->addWidget(status, 4, 0);
	grid->addWidget(snd, 4, 1);
	grid->addRowSpacing(3, 20);

	setCaption(tr("Register user"));
	resize(240, 150);

	connect(gadu, SIGNAL(registered(bool,UinType)), this, SLOT(registered(bool,UinType)));

	show();
}

void Register::doRegister() {
	kdebug("Register::doRegister()\n");
	
	if (pwd->text() != pwd2->text()) 
	{
		QMessageBox::warning(this, "Kadu", tr("Passwords do not match"), tr("OK"), 0, 0, 1);
		return;
	}

	if (!pwd->text().length()) 
	{
		QMessageBox::warning(this, "Kadu", tr("Please fill out all fields"), tr("OK"), 0, 0, 1);
		return;
	}

	TokenDialog *tokendialog = new TokenDialog();
	if (tokendialog->exec() != QDialog::Accepted) 
	{
		delete tokendialog;
		return;
	}
	
	QString Password, Email, Tokenid, Tokenval;
	tokendialog->getToken(Tokenid, Tokenval);
	delete tokendialog;

	Password = pwd->text();
	Email = mailedit->text();

	if (!gadu->doRegister(Password, Email, Tokenid, Tokenval))
	{
		status->setText(tr("Error"));
		return;
	}

	status->setText(tr("Registering"));

	setEnabled(false);
	
}

void Register::registered(bool ok, UinType uin)
{
	if (ok)
	{	
		status->setText(tr("Success!"));
		this->uin = uin;
		QMessageBox::information(this, "Kadu", tr("Registration was successful. Your new number is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(uin), tr("OK"), 0, 0, 1);
		ask();
		close();
	}
	else
	{
		status->setText(tr("Error"));
		setEnabled(true);
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

	QGridLayout *grid = new QGridLayout(this, 3, 2, 6, 5);

	QLabel *l_uin = new QLabel(tr("UIN"),this);
	uin = new QLineEdit(this);

	QLabel *l_pwd = new QLabel(tr("Password"),this);
	pwd = new QLineEdit(this);
	pwd->setEchoMode(QLineEdit::Password);

	QPushButton *snd = new QPushButton(this);
	snd->setText(tr("Unregister"));
	QObject::connect(snd, SIGNAL(clicked()), this, SLOT(doUnregister()));

	status = new QLabel(this);

	grid->addWidget(l_uin, 0, 0);
	grid->addWidget(uin, 0, 1);
	grid->addWidget(l_pwd, 1, 0);
	grid->addWidget(pwd, 1, 1);
	grid->addWidget(status, 2, 0);
	grid->addWidget(snd, 2, 1);
	grid->addRowSpacing(3, 20);

	setCaption(tr("Unregister user"));
	resize(240, 100);

	connect(gadu, SIGNAL(unregistered(bool)), this, SLOT(unregistered(bool)));
	show();
}

void Unregister::doUnregister() {
	kdebug("Unregister::doUnregister()\n");

	if (!uin->text().toUInt() || !pwd->text().length()) 
	{
		QMessageBox::warning(this, "Kadu", tr("Please fill out all fields"), tr("OK"), 0, 0, 1);
		return;
	}

	TokenDialog *tokendialog = new TokenDialog();
	if (tokendialog->exec() != QDialog::Accepted) 
	{
		delete tokendialog;
		return;
	}
	
	QString Tokenid, Tokenval;
	tokendialog->getToken(Tokenid, Tokenval);
	delete tokendialog;
	
	QString Password = pwd->text();

	if (!gadu->doUnregister(uin->text().toUInt(), Password, Tokenid, Tokenval))
	{
		status->setText(tr("Error"));
		return;
	}

	status->setText(tr("Unregistering"));

	setEnabled(false);

}

void Unregister::unregistered(bool ok)
{
	if (ok)
	{
		status->setText(tr("Success!"));
		QMessageBox::information(this, "Kadu", tr("Unregistation was successful. Now you don't have any GG number :("));
		close();
	}
	else
	{
		status->setText(tr("Error"));
		setEnabled(true);
	}
}

void Unregister::deleteConfig() {
	kdebug("Unregister::deleteConfig()\n");

	kdebug("Unregister::deleteConfig(): Deleting config file...\n");
	QFile::remove(ggPath("kadu.conf"));
	config_file.writeEntry("General","UIN",0);

	qApp->mainWidget()->setCaption(tr("No user"));

	kdebug("Unregister::deleteConfig(): Config file deleted\n");
}

