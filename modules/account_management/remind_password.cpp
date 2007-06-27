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
#include <qpushbutton.h>
#include <qvbox.h>
#include <qvgroupbox.h>

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
RemindPassword::RemindPassword(QDialog *parent, const char *name) : QHBox(parent, name, WDestructiveClose),
	emailedit(0), layoutHelper(new LayoutHelper())
{
	kdebugf();

	setCaption(tr("Remind password"));
	layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	QVBox *left = new QVBox(this);
	left->setMargin(10);
	left->setSpacing(10);

	QLabel *l_icon = new QLabel(left);
	QWidget *blank = new QWidget(left);
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBox *center = new QVBox(this);
	center->setMargin(10);
	center->setSpacing(10);

	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager->loadIcon("RemindPasswordWindowIcon"));
	l_info->setText(tr("This dialog box allows you to ask server to remind your current password."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)

	//our QVGroupBox
	QVGroupBox *vgb_email = new QVGroupBox(center);
	vgb_email->setTitle(tr("Email"));
	//end our QGroupBox

	// create needed fields

	new QLabel(tr("Email (as during registration):"), vgb_email);
	emailedit = new QLineEdit(vgb_email);

	// end create needed fields

	// buttons
	QHBox *bottom = new QHBox(center);
	QWidget *blank2 = new QWidget(bottom);
	bottom->setSpacing(5);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	QPushButton *pb_ok = new QPushButton(icons_manager->loadIcon("RemindPasswordButton"), tr("OK"), bottom, "ok");
	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons

	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(pb_ok, SIGNAL(clicked()), this, SLOT(start()));

	layoutHelper->addLabel(l_info);

	loadGeometry(this, "General", "RemindPasswordDialogGeometry", 0, 30, 355, 200);
	connect(gadu, SIGNAL(reminded(bool)), this, SLOT(reminded(bool)));

	kdebugf2();
}

RemindPassword::~RemindPassword()
{
	kdebugf();

	saveGeometry(this, "General", "RemindPasswordDialogGeometry");
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
