/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QGroupBox>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

#include "change_password.h"
#include "debug.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc.h"
#include "gadu.h"
#include "config_file.h"

/** @ingroup account_management
 * @{
 */
ChangePassword::ChangePassword(QDialog *parent, const char *name) : QWidget(parent, name, Qt::Window),
	emailedit(0), newpwd(0), newpwd2(0), layoutHelper(new LayoutHelper())
{
	kdebugf();

	setWindowTitle(tr("Change password / email"));
	setAttribute(Qt::WA_DeleteOnClose);

//	layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget();

	QLabel *l_icon = new QLabel;
	l_icon->setPixmap(icons_manager->loadPixmap("ChangePasswordWindowIcon"));

	QWidget *blank = new QWidget;
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBoxLayout *left_layout = new QVBoxLayout;
	left_layout->addWidget(l_icon);
	left_layout->addWidget(blank);
	left->setLayout(left_layout);

	QWidget *center = new QWidget;

	QLabel *l_info = new QLabel();

	l_info->setText(tr("This dialog box allows you to change your current password or e-mail."));
	l_info->setAlignment(Qt::TextWordWrap);
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

	// end create main QLabel widgets (icon and app info)

	//our QGroupBox
	QGroupBox *vgb_email = new QGroupBox(tr("Email"));
	QVBoxLayout *email_layout = new QVBoxLayout;
	vgb_email->setLayout(email_layout);

	QGroupBox *vgb_password = new QGroupBox(tr("Password"));
	QVBoxLayout *password_layout = new QVBoxLayout;
	vgb_password->setLayout(password_layout);
	//end our QGroupBox

	// create needed fields
	emailedit = new QLineEdit();
	email_layout->addWidget(new QLabel(tr("New email:")));
	email_layout->addWidget(emailedit);

	newpwd = new QLineEdit();
	newpwd->setEchoMode(QLineEdit::Password);
	password_layout->addWidget(new QLabel(tr("New password:")));
	password_layout->addWidget(newpwd);

	newpwd2 = new QLineEdit();
	newpwd2->setEchoMode(QLineEdit::Password);
	password_layout->addWidget(new QLabel(tr("Retype new password:")));
	password_layout->addWidget(newpwd2);

	// end create needed fields

	// buttons
	QWidget *bottom = new QWidget;

	QWidget *blank2 = new QWidget;
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	QPushButton *pb_ok = new QPushButton(icons_manager->loadIcon("ChangePasswordEmailButton"), tr("OK"), bottom, "ok");
	connect(pb_ok, SIGNAL(clicked()), this, SLOT(start()));

	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *bottom_layout = new QHBoxLayout;
	bottom_layout->addWidget(blank2);
	bottom_layout->addWidget(pb_ok);
	bottom_layout->addWidget(pb_close);

	bottom->setLayout(bottom_layout);

	// end buttons
	QVBoxLayout *center_layout = new QVBoxLayout;
	center_layout->addWidget(l_info);
	center_layout->addWidget(vgb_email);
	center_layout->addWidget(vgb_password);
	center_layout->setStretchFactor(vgb_password, 1);
	center_layout->addWidget(bottom);

	center->setLayout(center_layout);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(left);
	layout->addWidget(center);

	setLayout(layout);

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

//	saveGeometry(this, "General", "ChangePasswordDialogGeometry");
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
		MessageBox::msg(tr("Error data typed in required fields.\n\nPasswords typed in "
			"both fields (\"New password\" and \"Retype new password\") should be the same!"), false, "Warning", this);
		return;
	}

	QString password = QString(pwHash(config_file.readEntry("General", "Password")));
	QString newpassword = (newpwd->text().isEmpty() ? password : newpwd->text());

	setEnabled(false);
	gadu->changePassword(config_file.readNumEntry("General", "UIN"), emailedit->text(), password, newpassword);

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

/** @} */
