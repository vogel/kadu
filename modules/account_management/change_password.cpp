/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include "change_password.h"
#include "debug.h"
#include "icons_manager.h"
#include "misc.h"
#include "gadu.h"
#include "config_file.h"

ChangePassword::ChangePassword(QDialog *parent, const char *name) : QHBox(parent, name),
	emailedit(0), newpwd(0), newpwd2(0), layoutHelper(new LayoutHelper())
{
	kdebugf();
	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("Change password / email"));
	layout()->setResizeMode(QLayout::Minimum);

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
	l_icon->setPixmap(icons_manager->loadIcon("ChangePasswordWindowIcon"));
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
	QPushButton *pb_ok = new QPushButton(icons_manager->loadIcon("ChangePasswordEmailButton"), tr("OK"), bottom, "ok");
	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons

	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(pb_ok, SIGNAL(clicked()), this, SLOT(start()));

	layoutHelper->addLabel(l_info);
	loadGeometry(this, "General", "ChangePasswordDialogGeometry", 0, 30, 355, 350);
	connect(gadu, SIGNAL(passwordChanged(bool)), this, SLOT(passwordChanged(bool)));
	kdebugf2();
}

void ChangePassword::resizeEvent(QResizeEvent * /*e*/)
{
	layoutHelper->resizeLabels();
}

ChangePassword::~ChangePassword()
{
	kdebugf();
	saveGeometry(this, "General", "ChangePasswordDialogGeometry");
	delete layoutHelper;
	kdebugf2();
}

void ChangePassword::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void ChangePassword::start()
{
	kdebugf();

	if (newpwd->text() != newpwd2->text())
	{
		QMessageBox::information(0, tr("Change password"),
				tr("Error data typed in required fields.\n\nPasswords typed in "
				"both fields (\"New password\" and \"Retype new password\") "
				"should be the same!"), tr("OK"), 0, 0, 1);
		return;
	}

	QString mail = emailedit->text();
	QString password = QString(pwHash(config_file.readEntry("General", "Password")));
	QString newpassword = (newpwd->text().isEmpty() ? password : newpwd->text());

	setEnabled(false);
	gadu->changePassword(config_file.readNumEntry("General", "UIN"), mail, password, newpassword);
	kdebugf2();
}

void ChangePassword::passwordChanged(bool ok)
{
	kdebugf();
	if (ok)
	{
		config_file.writeEntry("General", "Password", pwHash(newpwd->text()));
		config_file.sync();
		close();
	}
	kdebugf2();
}

