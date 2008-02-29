/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <q3hbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QCloseEvent>

#include "icons_manager.h"
#include "message_box.h"
#include "debug.h"

const int MessageBox::OK       = 1;  // 00001
const int MessageBox::CANCEL   = 2;  // 00010
const int MessageBox::YES      = 4;  // 00100
const int MessageBox::NO       = 8;  // 01000

MessageBox::MessageBox(const QString& message, int components, bool modal, const QString &iconName, QWidget *parent)
	: QDialog(parent, NULL, modal, Qt::WType_TopLevel | Qt::WStyle_Customize | Qt::WStyle_DialogBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu | Qt::WDestructiveClose),
	message(message)
{
	kdebugf();

	Q3VBoxLayout* vbox = new Q3VBoxLayout(this, 0);
	vbox->setMargin(10);
	vbox->setSpacing(10);

	Q3HBoxLayout* hboxlabels = new Q3HBoxLayout(vbox);
	hboxlabels->addStretch(1);

	if (!iconName.isEmpty())
	{
		icon = new QLabel(this);
		icon->setPixmap(icons_manager->loadPixmap(iconName));
		hboxlabels->addWidget(icon, 0, Qt::AlignCenter);
	}

	QLabel* label = new QLabel(this);
	if (!message.isEmpty())
		label->setText(message);

	hboxlabels->addWidget(label, 0, Qt::AlignCenter);
	hboxlabels->addStretch(1);

	Q3HBoxLayout* hboxbuttons = new Q3HBoxLayout(vbox);
	Q3HBox* buttons = new Q3HBox(this);
	buttons->setSpacing(20);
	hboxbuttons->addWidget(buttons, 0, Qt::AlignCenter);

	if (components & OK)
		addButton(buttons, tr("&OK"), SLOT(okClicked()));

	if (components & YES)
		addButton(buttons, tr("&Yes"), SLOT(yesClicked()));

	if (components & NO)
		addButton(buttons, tr("&No"), SLOT(noClicked()));

	if (components & CANCEL)
		addButton(buttons, tr("&Cancel"), SLOT(cancelClicked()));

	buttons->setMaximumSize(buttons->sizeHint());
	kdebugf2();
}

MessageBox::~MessageBox()
{
	if (Boxes.contains(message))
		Boxes.remove(message);
}

void MessageBox::addButton(QWidget *parent, const QString &caption, const char *slot)
{
	QPushButton* b = new QPushButton(parent);
	b->setText(caption);
	connect(b, SIGNAL(clicked()), this, slot);
}

void MessageBox::closeEvent(QCloseEvent* e)
{
	e->ignore();
}

void MessageBox::okClicked()
{
	emit okPressed();
	accept();
}

void MessageBox::cancelClicked()
{
	emit cancelPressed();
	reject();
}

void MessageBox::yesClicked()
{
	emit yesPressed();
	accept();
}

void MessageBox::noClicked()
{
	emit noPressed();
	reject();
}

void MessageBox::status(const QString& message)
{
	MessageBox* m = new MessageBox(message);
	m->show();
	Boxes.insert(message,m);
	qApp->processEvents();
}

void MessageBox::msg(const QString& message, bool modal, const QString& iconName, QWidget *parent)
{
	MessageBox* m = new MessageBox(message, OK, modal, iconName, parent);

	if (modal)
		m->exec();
	else
		m->show();
}

bool MessageBox::ask(const QString& message, const QString& iconName, QWidget *parent)
{
	MessageBox* m = new MessageBox(message, YES|NO, true, iconName, parent);
	return (m->exec() == Accepted);
}

void MessageBox::close(const QString& message)
{
	if (Boxes.contains(message))
	{
		Boxes[message]->accept();
		Boxes.remove(message);
	}
}

QMap<QString,MessageBox*> MessageBox::Boxes;
