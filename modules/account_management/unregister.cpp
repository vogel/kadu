/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qfile.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "icons_manager.h"
#include "misc.h"
#include "unregister.h"

Unregister::Unregister(QDialog * /*parent*/, const char * /*name*/)
{
	kdebugf();
	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("Unregister user"));
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
	l_icon->setPixmap(icons_manager->loadIcon("UnregisterWindowIcon"));
	l_info->setText(tr("This dialog box allows you to unregister your account. Be aware of using this "
				"option. <font color=\"red\"><b>It will permanently delete your UIN and you will not be able to use "
				"it later!</b></font>"));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)

	//our QVGroupBox
	QVGroupBox *vgb_uinpass = new QVGroupBox(center);
	vgb_uinpass->setTitle(tr("UIN and password"));
	center->setStretchFactor(vgb_uinpass, 1);
	//end our QGroupBox

	// create needed fields

	new QLabel(tr("UIN:"), vgb_uinpass);
	uin = new QLineEdit(vgb_uinpass);

	new QLabel(tr("Password:"), vgb_uinpass);
	pwd = new QLineEdit(vgb_uinpass);
	pwd->setEchoMode(QLineEdit::Password);
	// end create needed fields

	// buttons
	QHBox *bottom = new QHBox(center);
	QWidget *blank2 = new QWidget(bottom);
	bottom->setSpacing(5);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	QPushButton *pb_unregister = new QPushButton(icons_manager->loadIcon("UnregisterAccountButton"), tr("Unregister"), bottom, "unregister");
	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons

	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(pb_unregister, SIGNAL(clicked()), this, SLOT(doUnregister()));
	connect(gadu, SIGNAL(unregistered(bool)), this, SLOT(unregistered(bool)));

	layoutHelper = new LayoutHelper();
	layoutHelper->addLabel(l_info);

 	loadGeometry(this, "General", "UnregisterDialogGeometry", 0, 30, 355, 340);
	kdebugf2();
}

Unregister::~Unregister()
{
	kdebugf();
	saveGeometry(this, "General", "UnregisterDialogGeometry");
	delete layoutHelper;
	kdebugf2();
}

void Unregister::resizeEvent(QResizeEvent *e)
{
	layoutHelper->resizeLabels();
}

void Unregister::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void Unregister::doUnregister() {
	kdebugf();

	if (!uin->text().toUInt() || pwd->text().isEmpty())
	{
		QMessageBox::warning(this, "Kadu", tr("Please fill out all fields"), tr("OK"), 0, 0, 1);
		return;
	}

	QString Password = pwd->text();
	setEnabled(false);
	gadu->unregisterAccount(uin->text().toUInt(), Password);
	kdebugf2();
}

void Unregister::unregistered(bool ok)
{
	kdebugf();
	if (ok)
	{
		QMessageBox::information(this, "Kadu", tr("Unregistation was successful. Now you don't have any GG number :("));
		close();
	}
	else
	{
		QMessageBox::warning(0, tr("Unregister user"),
				tr("An error has occured while unregistration. Please try again later."), tr("OK"), 0, 0, 1);
		setEnabled(true);
	}
	kdebugf2();
}

void Unregister::deleteConfig() {
	kdebugf();

	QFile::remove(ggPath("kadu.conf"));
	config_file.writeEntry("General","UIN",0);

	qApp->mainWidget()->setCaption(tr("No user"));

	kdebugf2();
}

