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

ChangePassword::ChangePassword(QDialog *parent, const char *name)
{
	kdebugf();
	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("Change password/email"));
	
	// create main QLabel widgets (icon and app info)
	QVBox *left=new QVBox(this);
	left->setMargin(10);
	left->setSpacing(10);
	
	QLabel *l_icon = new QLabel(left);
	QWidget *blank=new QWidget(left);
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
	
	QVBox *center=new QVBox(this);
	center->setMargin(10);
	center->setSpacing(10);
	
	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager.loadIcon("ChangePasswordWindowIcon"));
	l_info->setText(tr("This dialog box allows you to change your current password or e-mail."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)
	
	//our QVGroupBox
	QVGroupBox *vgb_email = new QVGroupBox(center);
	vgb_email->setTitle(tr("Email"));
	QVGroupBox *vgb_password = new QVGroupBox(center);
	vgb_password->setTitle(tr("Password"));
	center->setStretchFactor(vgb_password, 1);
	//end our QGroupBox
	
	// create needed fields
	
	new QLabel(tr("New email:"), vgb_email);
	emailedit = new QLineEdit(vgb_email);
	
	new QLabel(tr("New password:"), vgb_password);
	newpwd = new QLineEdit(vgb_password);
	newpwd->setEchoMode(QLineEdit::Password);
	
	new QLabel(tr("Retype new password:"), vgb_password);
	newpwd2 = new QLineEdit(vgb_password);
	newpwd2->setEchoMode(QLineEdit::Password);

	// end create needed fields

	// buttons
	QHBox *bottom = new QHBox(center);
	QWidget *blank2 = new QWidget(bottom);
	bottom->setSpacing(5);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	QPushButton *pb_ok = new QPushButton(icons_manager.loadIcon("ChangePasswordEmailButton"), tr("OK"), bottom, "ok");
	QPushButton *pb_close = new QPushButton(icons_manager.loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons
	
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(pb_ok, SIGNAL(clicked()), this, SLOT(start()));
	
 	loadGeometry(this, "General", "RemindPasswordDialogGeometry", 0, 0, 300, 120);
	connect(gadu, SIGNAL(passwordChanged(bool)), this, SLOT(passwordChanged(bool)));
}

ChangePassword::~ChangePassword()
{
	kdebugf();
	saveGeometry(this, "General", "RemindPasswordDialogGeometry");
}

void ChangePassword::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void ChangePassword::start() 
{
	kdebug("ChangePassword::start()\n");

	if (newpwd->text() != newpwd2->text())
	{
		QMessageBox::information(0, tr("Change password"),
				tr("Error data typed in required fields.\n\nPasswords typed in "
				"both fields (\"New password\" and \"Retype new password\") "
				"should be the same!"), tr("OK"), 0, 0, 1);
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
		close();
	}
}

