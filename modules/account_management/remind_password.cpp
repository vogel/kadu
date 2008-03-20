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

#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc.h"
#include "remind_password.h"

/**
 * @ingroup account_management
 * @{
 */
RemindPassword::RemindPassword(QDialog *parent, const char *name) : QWidget(parent, name, Qt::Window),
	emailedit(0), layoutHelper(new LayoutHelper())
{
	kdebugf();

	setWindowTitle(tr("Remind password"));
	setAttribute(Qt::WA_DeleteOnClose);
//	layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget();

	QLabel *l_icon = new QLabel;
	l_icon->setPixmap(icons_manager->loadPixmap("RemindPasswordWindowIcon"));

	QWidget *blank = new QWidget;
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBoxLayout *left_layout = new QVBoxLayout;
	left_layout->addWidget(l_icon);
	left_layout->addWidget(blank);
	left->setLayout(left_layout);

	QWidget *center = new QWidget;

	QLabel *l_info = new QLabel();

	l_info->setText(tr("This dialog box allows you to ask server to remind your current password."));
	l_info->setAlignment(Qt::WordBreak);
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

	// end create main QLabel widgets (icon and app info)

	//our QVGroupBox
	QGroupBox *vgb_email = new QGroupBox(tr("Email"));
	QVBoxLayout *email_layout = new QVBoxLayout;
	//end our QGroupBox

	// create needed fields

	emailedit = new QLineEdit();
	email_layout->addWidget(new QLabel(tr("Email (as during registration):")));
	email_layout->addWidget(emailedit);

	vgb_email->setLayout(email_layout);
	// end create needed fields

	// buttons
	QWidget *bottom = new QWidget;

	QWidget *blank2 = new QWidget;
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	QPushButton *pb_ok = new QPushButton(icons_manager->loadIcon("RemindPasswordButton"), tr("OK"), bottom, "ok");
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
	center_layout->addWidget(bottom);

	center->setLayout(center_layout);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(left);
	layout->addWidget(center);

	setLayout(layout);

	layoutHelper->addLabel(l_info);

	loadGeometry(this, "General", "RemindPasswordDialogGeometry", 0, 30, 355, 200);
	connect(gadu, SIGNAL(reminded(bool)), this, SLOT(reminded(bool)));

	kdebugf2();
}

RemindPassword::~RemindPassword()
{
	kdebugf();

//	saveGeometry(this, "General", "RemindPasswordDialogGeometry");
	delete layoutHelper;

	kdebugf2();
}

void RemindPassword::resizeEvent(QResizeEvent * /*e*/)
{
	layoutHelper->resizeLabels();
}

void RemindPassword::keyPressEvent(QKeyEvent* ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void RemindPassword::start()
{
	kdebugf();

	setEnabled(false);
	gadu->remindPassword(config_file.readNumEntry("General", "UIN"), emailedit->text());

	kdebugf2();
}

void RemindPassword::reminded(bool ok)
{
	kdebugf();

	if (ok)
		MessageBox::msg(tr("Your password has been send on your email"), false, "Information", this);
	else
		MessageBox::msg(tr("Error during remind password"), false, "Critical", this);
	close();

	kdebugf2();
}

/** @} */
