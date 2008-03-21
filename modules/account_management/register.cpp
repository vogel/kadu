/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <QCheckBox>
#include <QGroupBox>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

#include "config_file.h"
#include "debug.h"
#include "html_document.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc.h"
#include "register.h"

#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @ingroup account_management
 * @{
 */
void Register::createConfig()
{
	kdebugf();

	char *home = getenv("HOME");
	struct passwd *pw;

	kdebugmf(KDEBUG_INFO, "$HOME=%s\n", home);
	if (!home) {
		if (!(pw = getpwuid(getuid())))
			return;
		home = pw->pw_dir;
	}

	struct stat buf;
	QString ggpath = ggPath(QString::null);
	stat(ggpath.local8Bit(), &buf);
	if (S_ISDIR(buf.st_mode))
		kdebugmf(KDEBUG_INFO, "Directory %s exists\n", (const char *)ggpath.local8Bit());
	else
	{
		kdebugmf(KDEBUG_INFO, "Creating directory\n");
		if (mkdir(ggpath.local8Bit(), 0700) != 0 )
		{
			perror("mkdir");
			return;
		}
	}

	kdebugmf(KDEBUG_INFO, "Writing config files...\n");
	config_file.sync();

	qApp->mainWidget()->setWindowTitle(QString("Kadu: %1").arg((UinType)config_file.readNumEntry("General","UIN")));

	kdebugf2();
}

Register::Register(QDialog *parent, const char *name) : QWidget(parent, name, Qt::Window),
	pwd(0), pwd2(0), mailedit(0), uin(0), cb_updateconfig(0),
	layoutHelper(new LayoutHelper())
{
	kdebugf();

	setWindowTitle(tr("Register user"));
	setAttribute(Qt::WA_DeleteOnClose);
//	layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget();

	QLabel *l_icon = new QLabel;
	l_icon->setPixmap(icons_manager->loadPixmap("RegisterWindowIcon"));

	QWidget *blank = new QWidget;
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBoxLayout *left_layout = new QVBoxLayout;
	left_layout->addWidget(l_icon);
	left_layout->addWidget(blank);
	left->setLayout(left_layout);

	QWidget *center = new QWidget;

	QLabel *l_info = new QLabel();

	l_info->setText(tr("This dialog box allows you to register a new account."));
	l_info->setAlignment(Qt::TextWordWrap);
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

	// end create main QLabel widgets (icon and app info)

	//our QVGroupBox
	QGroupBox *vgb_email = new QGroupBox(tr("Email"));
	QVBoxLayout *email_layout = new QVBoxLayout;
	vgb_email->setLayout(email_layout);

	QGroupBox *vgb_password = new QGroupBox(tr("Password"));
	QVBoxLayout *password_layout = new QVBoxLayout;
	vgb_password->setLayout(password_layout);

	//end our QGroupBox

	// create needed fields
	mailedit = new QLineEdit();
	email_layout->addWidget(new QLabel(tr("New email:")));
	email_layout->addWidget(mailedit);

	pwd = new QLineEdit();
	pwd->setEchoMode(QLineEdit::Password);
	password_layout->addWidget(new QLabel(tr("New password:")));
	password_layout->addWidget(pwd);

	pwd2 = new QLineEdit();
	pwd2->setEchoMode(QLineEdit::Password);
	password_layout->addWidget(new QLabel(tr("Retype new password:")));
	password_layout->addWidget(pwd2);
	// end create needed fields

	cb_updateconfig = new QCheckBox;
	cb_updateconfig->setChecked(true);
	cb_updateconfig->setText(tr("Create config file"));
	cb_updateconfig->setToolTip(tr("Write the newly obtained UIN and password into a clean configuration file\nThis will erase your current config file contents if you have one"));

	// buttons
	QWidget *bottom = new QWidget;

	QWidget *blank2 = new QWidget;
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	QPushButton *pb_register = new QPushButton(icons_manager->loadIcon("RegisterAccountButton"), tr("Register"), bottom, "register");
	connect(pb_register, SIGNAL(clicked()), this, SLOT(doRegister()));

	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *bottom_layout = new QHBoxLayout;
	bottom_layout->addWidget(blank2);
	bottom_layout->addWidget(pb_register);
	bottom_layout->addWidget(pb_close);

	bottom->setLayout(bottom_layout);

	// end buttons

	QVBoxLayout *center_layout = new QVBoxLayout;
	center_layout->addWidget(l_info);
	center_layout->addWidget(vgb_email);
	center_layout->addWidget(vgb_password);
	center_layout->setStretchFactor(vgb_password, 1);
	center_layout->addWidget(cb_updateconfig);
	center_layout->addWidget(bottom);

	center->setLayout(center_layout);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(left);
	layout->addWidget(center);

	setLayout(layout);

	connect(gadu, SIGNAL(registered(bool, UinType)), this, SLOT(registered(bool, UinType)));

	layoutHelper->addLabel(l_info);

 	loadGeometry(this, "General", "RegisterDialogGeometry", 0, 30, 400, 400);

	kdebugf2();
}

Register::~Register()
{
	kdebugf();

//	saveGeometry(this, "General", "RegisterDialogGeometry");
	delete layoutHelper;

	kdebugf2();
}

void Register::resizeEvent(QResizeEvent * /*e*/)
{
	layoutHelper->resizeLabels();
}

void Register::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void Register::doRegister()
{
	kdebugf();

	if (pwd->text() != pwd2->text())
	{
		MessageBox::msg(tr("Error data typed in required fields.\n\nPasswords typed in "
			"both fields (\"New password\" and \"Retype new password\") should be the same!"), false, "Warning", this);
		return;
	}

	if (pwd->text().isEmpty() || mailedit->text().isEmpty())
	{
		MessageBox::msg(tr("Please fill out all fields"), false, "Warning", this);
		return;
	}

	if (mailedit->text().find(HtmlDocument::mailRegExp()) == -1)
	{
		MessageBox::msg(tr("Email address you have entered is not valid"), false, "Warning", this);
		return;
	}

	setEnabled(false);
	gadu->registerAccount(mailedit->text(), pwd->text());

	kdebugf2();
}

void Register::registered(bool ok, UinType uin)
{
	kdebugf();

	if (ok)
	{
		this->uin = uin;
		MessageBox::msg(tr("Registration was successful. Your new number is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(uin), false, "Information", this);
		ask();
		close();
	}
	else
	{
		MessageBox::msg(tr("An error has occured while registration. Please try again later."), false, "Warning", this);
		setEnabled(true);
	}

	kdebugf2();
}


void Register::ask()
{
	kdebugf();

	if (cb_updateconfig->isChecked())
	{
		config_file.writeEntry("General", "UIN", (int)uin);
		config_file.writeEntry("General", "Password", pwHash(pwd->text()));
		createConfig();
	}

	kdebugf2();
}

/** @} */
