/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QMap>
#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "debug.h"
#include "icons-manager.h"

#include "message-box.h"

const int MessageBox::OK       = 1;  // 00001
const int MessageBox::CANCEL   = 2;  // 00010
const int MessageBox::YES      = 4;  // 00100
const int MessageBox::NO       = 8;  // 01000

MessageBox::MessageBox(const QString &message, int components, bool modal, const QString &iconName, QWidget *parent)
	: QDialog(parent, Qt::Window | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
	message(message)
{
	kdebugf();

	setWindowTitle("Kadu");
	setAttribute(Qt::WA_DeleteOnClose);
	
	setModal(modal);

	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->setContentsMargins(10, 10, 10, 10);
	vbox->setSpacing(10);

	QHBoxLayout *hboxlabels = new QHBoxLayout();
	hboxlabels->addStretch(1);
	vbox->addLayout(hboxlabels);

	if (!iconName.isEmpty())
	{
		icon = new QLabel(this);
		icon->setPixmap(IconsManager::instance()->loadPixmap(iconName));
		hboxlabels->addWidget(icon, 0, Qt::AlignCenter);
	}

	QLabel *label = new QLabel(this);
	if (!message.isEmpty())
		label->setText(message);

	hboxlabels->addWidget(label, 0, Qt::AlignCenter);
	hboxlabels->addStretch(1);

	QHBoxLayout *hboxbuttons = new QHBoxLayout();
	vbox->addLayout(hboxbuttons);

	QWidget *buttons = new QWidget;
	QHBoxLayout *buttons_layout = new QHBoxLayout(buttons);
	buttons_layout->setSpacing(20);

	if (components & OK)
		addButton(buttons_layout, tr("&OK"), SLOT(okClicked()));

	if (components & YES)
		addButton(buttons_layout, tr("&Yes"), SLOT(yesClicked()));

	if (components & NO)
		addButton(buttons_layout, tr("&No"), SLOT(noClicked()));

	if (components & CANCEL)
		addButton(buttons_layout, tr("&Cancel"), SLOT(cancelClicked()));

	hboxbuttons->addWidget(buttons, 0, Qt::AlignCenter);
 	buttons->setMaximumSize(buttons_layout->sizeHint());
	kdebugf2();
}

MessageBox::~MessageBox()
{
	if (Boxes.contains(message))
		Boxes.remove(message);
}

void MessageBox::addButton(QBoxLayout *parent, const QString &caption, const char *slot)
{
	QPushButton *b = new QPushButton;
	b->setText(caption);
	parent->addWidget(b);
	connect(b, SIGNAL(clicked()), this, slot);
}

void MessageBox::closeEvent(QCloseEvent *e)
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

void MessageBox::status(const QString &message)
{
	MessageBox *m = new MessageBox(message);
	m->show();
	Boxes.insert(message,m);
	qApp->processEvents();
}

void MessageBox::msg(const QString &message, bool modal, const QString &iconName, QWidget *parent)
{
	MessageBox *m = new MessageBox(message, OK, modal, iconName, parent);

	if (modal)
		m->exec();
	else
		m->show();
}

bool MessageBox::ask(const QString &message, const QString &iconName, QWidget *parent)
{
	MessageBox *m = new MessageBox(message, YES|NO, true, iconName, parent);
	return (m->exec() == Accepted);
}

void MessageBox::close(const QString &message)
{
	if (Boxes.contains(message))
	{
		Boxes[message]->accept();
		Boxes.remove(message);
	}
}

QMap<QString,MessageBox*> MessageBox::Boxes;
