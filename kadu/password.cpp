#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>

#include <stdlib.h>

#include "misc.h"
#include "gadu.h"
#include "config_file.h"
#include "config_dialog.h"
#include "debug.h"
#include "password.h"

RemindPassword::RemindPassword() {
	connect(gadu, SIGNAL(reminded(bool)), this, SLOT(reminded(bool)));
}

RemindPassword::~RemindPassword() {
}

void RemindPassword::start() {
	kdebug("RemindPassword::start()\n");

	TokenDialog *tokendialog = new TokenDialog();
	if (tokendialog->exec() != QDialog::Accepted)
	{
		delete tokendialog;
		return;
	}

	QString Tokenid, Tokenval;
	tokendialog->getToken(Tokenid, Tokenval);
	delete tokendialog;

	gadu->doRemind(config_file.readNumEntry("General", "UIN"), Tokenid, Tokenval);
}

void RemindPassword::reminded(bool ok)
{
	if (ok)
		QMessageBox::information(0, tr("Remind password"),
				tr("Password reminded"), tr("OK"), 0, 0, 1);
	else
		QMessageBox::information(0, tr("Remind password"),
				tr("Error during remind password"), tr("OK"), 0, 0, 1);

	deleteLater();
}

ChangePassword::ChangePassword(QDialog *parent, const char *name) : QDialog(parent, name, FALSE, Qt::WDestructiveClose) {
	kdebug("ChangePassword::ChangePassword()\n");

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

	connect(gadu, SIGNAL(passwordChanged(bool)), this, SLOT(passwordChanged(bool)));
}

void ChangePassword::start() {
	kdebug("ChangePassword::start()\n");

	if (newpwd->text() != newpwd2->text())
	{
		status->setText(tr("Bad data"));
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

	QString mail = emailedit->text();
	QString password = QString(pwHash(config_file.readEntry("General", "Password")));
	QString newpassword = (newpwd->text().length() ? newpwd->text() : password);
	
	if (gadu->doChangePassword(config_file.readNumEntry("General", "UIN"), mail, password, newpassword, Tokenid, Tokenval))
		setEnabled(false);
}

void ChangePassword::passwordChanged(bool ok)
{
	if (ok)
	{
		config_file.writeEntry("General", "Password", pwHash(newpwd->text()));
		config_file.sync();
		accept();
	}
	else
	{
		status->setText(tr("Error"));
	}
}

