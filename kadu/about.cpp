/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qpixmap.h>
#include <qlabel.h>
#include <qtextbrowser.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qtextcodec.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qsizepolicy.h>

#include "about.h"
#include "misc.h"
#include "kadu-config.h"
#include "config_file.h"
#include "debug.h"

About::About() {
	kdebugf();
	// set window properties and flags
	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("About"));
	setMinimumSize(400, c00);
	// end set window properties and flags
	
	// create main QLabel widgets (icon and app info)
	QVBox *left=new QVBox(this);
	QLabel *l_icon = new QLabel(left);
	QWidget *blank=new QWidget(left);
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
	
	QVBox *center=new QVBox(this);
	
	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager.loadIcon("AboutIcon"));
	l_info->setText(QString("<span style=\"font-size: 12pt\">Kadu ") + QString(VERSION)
		+ QString(tr("<br>(c) 2001-2004 Kadu Team</span>")));
	// end create main QLabel widgets (icon and app info)
	
	// our TabWidget
	QTabWidget *tw_about = new QTabWidget(center);
	// end our TabWidget
	
	// create our info widgets
	// authors
	QTextBrowser *tb_authors = new QTextBrowser(tw_about);
	tb_authors->setFrameStyle(QFrame::NoFrame);
	tb_authors->setTextFormat(Qt::PlainText);
	tb_authors->setWordWrap(QTextEdit::NoWrap);
	tb_authors->setText(loadFile("AUTHORS"));
	
	// authors
	QTextBrowser *tb_thanks = new QTextBrowser(tw_about);
	tb_thanks->setFrameStyle(QFrame::NoFrame);
	tb_thanks->setTextFormat(Qt::PlainText);
	tb_thanks->setWordWrap(QTextEdit::NoWrap);
	tb_thanks->setText(loadFile("THANKS"));
	
	// license
	QTextBrowser *tb_license = new QTextBrowser(tw_about);
	tb_license->setFrameStyle(QFrame::NoFrame);
	tb_license->setTextFormat(Qt::PlainText);
	tb_license->setWordWrap(QTextEdit::NoWrap);
	tb_license->setText(loadFile("COPYING"));
	
	// changelog
	QTextBrowser *tb_changelog = new QTextBrowser(tw_about);
	tb_changelog->setFrameStyle(QFrame::NoFrame);
	tb_changelog->setTextFormat(Qt::PlainText);
	tb_changelog->setWordWrap(QTextEdit::NoWrap);
	tb_changelog->setText(loadFile("ChangeLog"));
	
	// add tabs
	tw_about->addTab(tb_authors, tr("A&uthors"));
	tw_about->addTab(tb_thanks, tr("&Thanks"));
	tw_about->addTab(tb_license, tr("&License")); 
	tw_about->addTab(tb_changelog, tr("&ChangeLog")); 
	// end create our info widgets
	
	// close button
	QHBox *bottom=new QHBox(center);
	QWidget *blank2=new QWidget(bottom);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	QPushButton *pb_close = new QPushButton(icons_manager.loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	// end close button

	QRect def_rect(20, 20, 580, 400);
	config_file.addVariable("General", "AboutGeometry", def_rect);

	QRect geom=config_file.readRectEntry("General", "AboutGeometry");
	resize(geom.width(),geom.height());
	move(geom.x(),geom.y());
}

About::~About()
{
	kdebugf();
	QRect geom;
	geom.setX(pos().x());
	geom.setY(pos().y());
	geom.setWidth(size().width());
	geom.setHeight(size().height());
	
	config_file.writeEntry("General", "AboutGeometry", geom);
}

QString About::loadFile(const QString &name)
{
	QString data;
	QFile file(dataPath("kadu/" + name));
	if (!file.open(IO_ReadOnly))
		return QString::null;
	QTextStream str(&file);
	str.setCodec(QTextCodec::codecForName("ISO8859-2"));
	data = str.read();
	file.close();
	return data;
}
