/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
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
#include "unregister.h"

/**
 * @ingroup account_management
 * @{
 */
Unregister::Unregister(QDialog *parent, const char *name) : QWidget(parent, name, Qt::Window),
	uin(0), pwd(0), layoutHelper(new LayoutHelper())
{
	kdebugf();

	setWindowTitle(tr("Unregister user"));
	setAttribute(Qt::WA_DeleteOnClose);

//	layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget();

	QLabel *l_icon = new QLabel;
	l_icon->setPixmap(icons_manager->loadPixmap("UnregisterWindowIcon"));

	QWidget *blank = new QWidget;
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBoxLayout *left_layout = new QVBoxLayout;
	left_layout->addWidget(l_icon);
	left_layout->addWidget(blank);
	left->setLayout(left_layout);

	QWidget *center = new QWidget;

	QLabel *l_info = new QLabel();
//TODO:fix this
	l_info->setText(tr("This dialog box allows you to unregister your account. Be aware of using this "
				"option. <font color=\"red\"><b>It will permanently delete your UIN and you will not be able to use "
				"it later!</b></font>"));
	l_info->setAlignment(Qt::WordBreak);
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	// end create main QLabel widgets (icon and app info)

	//our QGroupBox
	QGroupBox *vgb_uinpass = new QGroupBox(tr("UIN and password"));
	QVBoxLayout *uinpass_layout = new QVBoxLayout;
	vgb_uinpass->setLayout(uinpass_layout);

	//end our QGroupBox

	// create needed fields
	uin = new QLineEdit();
	uinpass_layout->addWidget(new QLabel(tr("UIN:")));
	uinpass_layout->addWidget(uin);

	pwd = new QLineEdit();
	pwd->setEchoMode(QLineEdit::Password);
	uinpass_layout->addWidget(new QLabel(tr("Password:")));
	uinpass_layout->addWidget(pwd);
	// end create needed fields

	// buttons
	QWidget *bottom = new QWidget;

	QWidget *blank2 = new QWidget;
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	QPushButton *pb_unregister = new QPushButton(icons_manager->loadIcon("UnregisterAccountButton"), tr("Unregister"), bottom, "unregister");
	connect(pb_unregister, SIGNAL(clicked()), this, SLOT(doUnregister()));

	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *bottom_layout = new QHBoxLayout;
	bottom_layout->addWidget(blank2);
	bottom_layout->addWidget(pb_unregister);
	bottom_layout->addWidget(pb_close);

	bottom->setLayout(bottom_layout);

	// end buttons

	QVBoxLayout *center_layout = new QVBoxLayout;
	center_layout->addWidget(l_info);
	center_layout->addWidget(vgb_uinpass);
	center_layout->addStretch(1);
	center_layout->addWidget(bottom);

	center->setLayout(center_layout);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(left);
	layout->addWidget(center);

	setLayout(layout);



	connect(gadu, SIGNAL(unregistered(bool)), this, SLOT(unregistered(bool)));

	layoutHelper->addLabel(l_info);

 	loadGeometry(this, "General", "UnregisterDialogGeometry", 0, 30, 355, 340);

	kdebugf2();
}

Unregister::~Unregister()
{
	kdebugf();

//	saveGeometry(this, "General", "UnregisterDialogGeometry");
	delete layoutHelper;

	kdebugf2();
}

void Unregister::resizeEvent(QResizeEvent * /*e*/)
{
	layoutHelper->resizeLabels();
}

void Unregister::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void Unregister::doUnregister()
{
	kdebugf();

	if (!uin->text().toUInt() || pwd->text().isEmpty())
	{
		MessageBox::msg(tr("Please fill out all fields"), false, "Warning", this);
		return;
	}

	setEnabled(false);
	gadu->unregisterAccount(uin->text().toUInt(), pwd->text());
	kdebugf2();
}

void Unregister::unregistered(bool ok)
{
	kdebugf();

	if (ok)
	{
		MessageBox::msg(tr("Unregistation was successful. Now you don't have any GG number :("), false, "Information", this);
		close();
	}
	else
	{
		MessageBox::msg(tr("An error has occured while unregistration. Please try again later."), false, "Critical", this);
		setEnabled(true);
	}

	kdebugf2();
}

void Unregister::deleteConfig()
{
	kdebugf();

	config_file.writeEntry("General", "UIN", 0);

	qApp->mainWidget()->setWindowTitle(tr("No user"));

	kdebugf2();
}

/** @} */
