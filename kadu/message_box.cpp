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
#include <qprogressbar.h>
#include <qpushbutton.h>

#include "message_box.h"
#include "misc.h"
#include "debug.h"

const int MessageBox::OK       = 1;  // 00001
const int MessageBox::CANCEL   = 2;  // 00010
const int MessageBox::YES      = 4;  // 00100
const int MessageBox::NO       = 8;  // 01000
const int MessageBox::PROGRESS = 16; // 10000

MessageBox::MessageBox(const QString& message,int components,bool modal)
	: QDialog(NULL,NULL,modal,WType_TopLevel|WStyle_Customize|WStyle_DialogBorder|WStyle_Title|WStyle_SysMenu)
{
	kdebugf();
	_pixmap = 0;
	_grid=new QGridLayout(this,1,2);
	
	QVBoxLayout* vbox=new QVBoxLayout(0);
	_grid->addLayout(vbox,0,1);
	vbox->setMargin(20);
	vbox->setSpacing(20);
	if(message.length()>0)
	{
		QLabel* l=new QLabel(this);
		l->setText(message);
		vbox->addWidget(l,0,AlignCenter);
	}
	if(components&PROGRESS)
	{
		Progress=new QProgressBar(this);
		vbox->addWidget(Progress,0,AlignCenter);
	}
	else
		Progress=NULL;

	if(!(components&(~PROGRESS)))
		return;

	QHBoxLayout* hbox=new QHBoxLayout(vbox);
	QHBox* buttons=new QHBox(this);
	buttons->setSpacing(20);
	hbox->addWidget(buttons,0,AlignCenter);
	if(components&OK)
	{
		QPushButton* b=new QPushButton(buttons);
		b->setText(tr("&OK"));
		connect(b,SIGNAL(clicked()),this,SLOT(okClicked()));
	}
	if(components&YES)
	{
		QPushButton* b=new QPushButton(buttons);
		b->setText(tr("&Yes"));
		connect(b,SIGNAL(clicked()),this,SLOT(yesClicked()));
	}
	if(components&NO)
	{
		QPushButton* b=new QPushButton(buttons);
		b->setText(tr("&No"));
		connect(b,SIGNAL(clicked()),this,SLOT(noClicked()));
	}
	if(components&CANCEL)
	{
		QPushButton* b=new QPushButton(buttons);
		b->setText(tr("&Cancel"));
		connect(b,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	}
	buttons->setMaximumSize(buttons->sizeHint());
	kdebugf2();
}

void MessageBox::setIcon(const QPixmap & pixmap)
{
	if (_pixmap == 0) {
		_pixmap = new QLabel(this);
		_pixmap->setPixmap(pixmap);
		_grid->addWidget(_pixmap,0,0);
		}

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

void MessageBox::setTotalSteps(int s)
{
	if(Progress!=NULL)
		Progress->setTotalSteps(s);
}

void MessageBox::setProgress(int p)
{
	if(Progress!=NULL)
		Progress->setProgress(p);
}

void MessageBox::status(const QString& message)
{
	MessageBox* m=new MessageBox(message);
	m->show();
	Boxes.insert(message,m);
	qApp->processEvents();
}

void MessageBox::msg(const QString& message,bool modal)
{
	MessageBox* m=new MessageBox(message,OK,modal);
	if (modal)
		m->exec();
	else
		m->show();
}

void MessageBox::wrn(const QString& message,bool modal)
{
	MessageBox* m=new MessageBox(message,OK,modal);
	m->setIcon(icons_manager.loadIcon("Warning"));
	if (modal)
		m->exec();
	else
		m->show();
}

bool MessageBox::ask(const QString& message)
{
	MessageBox* m=new MessageBox(message,YES|NO,true);
	return (m->exec()==Accepted);
}

void MessageBox::progress(const QString& message, const QObject* receiver,
	const char* slot, int total_steps)
{
	kdebugf();
	MessageBox* m;
	if(receiver!=0&&slot!=0)
	{
		m=new MessageBox(message,PROGRESS|CANCEL);
		connect(m,SIGNAL(cancelPressed()),receiver,slot);
	}
	else
		m=new MessageBox(message,PROGRESS);
	m->setTotalSteps(total_steps);
	m->show();
	Boxes.insert(message,m);	
	qApp->processEvents();
	kdebugf2();
}

void MessageBox::progress(const QString& message,int progress)
{
	if(Boxes.contains(message))
	{
		MessageBox* m=Boxes[message];
		m->setProgress(progress);
		qApp->processEvents();
	}
}

void MessageBox::close(const QString& message)
{
	if(Boxes.contains(message))
	{
		Boxes[message]->accept();
		Boxes.remove(message);
	}
}

QMap<QString,MessageBox*> MessageBox::Boxes;
