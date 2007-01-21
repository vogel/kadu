/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

#include "icons_manager.h"
#include "message_box.h"
#include "debug.h"

const int MessageBox::OK       = 1;  // 00001
const int MessageBox::CANCEL   = 2;  // 00010
const int MessageBox::YES      = 4;  // 00100
const int MessageBox::NO       = 8;  // 01000

MessageBox::MessageBox(const QString& message, int components, bool modal, const QString &iconName)
	: QDialog(NULL, NULL, modal, WType_TopLevel | WStyle_Customize | WStyle_DialogBorder | WStyle_Title | WStyle_SysMenu | WDestructiveClose),
	message(message)
{
	kdebugf();

	QVBoxLayout* vbox = new QVBoxLayout(this, 0);
	vbox->setMargin(10);
	vbox->setSpacing(10);

	labels = new QHBox(this);
	labels->setSpacing(10);
	vbox->addWidget(labels, 0, AlignCenter);

	if (!iconName.isEmpty())
	{
		icon = new QLabel(labels);
		icon->setPixmap(icons_manager->loadIcon(iconName));
		vbox->addWidget(icon, 0, AlignCenter);
	}

	QLabel* label = new QLabel(labels);
	if (!message.isEmpty())
		label->setText(message);

	QHBoxLayout* hboxbuttons = new QHBoxLayout(vbox);
	QHBox* buttons = new QHBox(this);
	buttons->setSpacing(20);
	hboxbuttons->addWidget(buttons, 0, AlignCenter);

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

void MessageBox::msg(const QString& message,bool modal)
{
	MessageBox* m = new MessageBox(message, OK, modal);

	if (modal)
		m->exec();
	else
		m->show();
}

void MessageBox::wrn(const QString& message,bool modal)
{
	MessageBox* m = new MessageBox(message, OK, modal, "Warning");

	if (modal)
		m->exec();
	else
		m->show();
}

bool MessageBox::ask(const QString& message)
{
	MessageBox* m = new MessageBox(message, YES|NO, true);
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
