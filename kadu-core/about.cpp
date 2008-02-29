/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qglobal.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <q3popupmenu.h>
#include <q3textstream.h>
#include <q3textbrowser.h>
#include <q3textedit.h>
#include <q3vbox.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QMouseEvent>
#include <Q3Frame>
#include <QKeyEvent>

#include "about.h"
#include "debug.h"
#include "icons_manager.h"
#include "misc.h"

class KaduLink : public QLabel
{
	public:
		KaduLink(QWidget *parent) : QLabel(parent)
		{
			setText("<a href=\"http://www.kadu.net/\">www.kadu.net</a>");
			setCursor(QCursor(Qt::PointingHandCursor));
			setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
		}
	protected:
		virtual void mousePressEvent(QMouseEvent *)
		{
			openWebBrowser("http://www.kadu.net/");
		}
};

About::About(QWidget *parent, const char *name) : Q3HBox(parent, name/*, Qt::WType_TopLevel | Qt::WDestructiveClose*/),
	layoutHelper(new LayoutHelper())
{
	kdebugf();

	// set window properties and flags
// 	setCaption(tr("About"));
// 	layout()->setResizeMode(QLayout::Minimum);
	// end set window properties and flags

	// create main QLabel widgets (icon and app info)
	Q3VBox *left=new Q3VBox(this);
	left->setMargin(10);
	left->setSpacing(10);

	QLabel *l_icon = new QLabel(left);
	l_icon->setPixmap(icons_manager->loadPixmap("AboutIcon"));

	QWidget *blank = new QWidget(left);
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	Q3VBox *center = new Q3VBox(this);
	center->setMargin(10);
	center->setSpacing(10);

	Q3HBox *texts = new Q3HBox(center);
	QLabel *l_info = new QLabel(texts);
	l_info->setText(QString("<span style=\"font-size: 12pt\">Kadu %1 %2<br />(c) 2001-2008 Kadu Team</span>").arg(VERSION)
			.arg(strlen(detailed_version) > 0 ? ("(" + QString(detailed_version) + ")") : QString::null));
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

//	QWidget *blank3 = new QWidget(texts);
//	blank3->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

	new KaduLink(texts);
	// end create main QLabel widgets (icon and app info)

	// our TabWidget
	QTabWidget *tw_about = new QTabWidget(center);
	// end our TabWidget

	// create our info widgets
	// authors
	Q3TextBrowser *tb_authors = new Q3TextBrowser(tw_about);
	tb_authors->setFrameStyle(Q3Frame::NoFrame);
	tb_authors->setTextFormat(Qt::PlainText);
	tb_authors->setWordWrap(Q3TextEdit::NoWrap);
	tb_authors->setText(loadFile("AUTHORS"));

	// people to thank
	Q3TextBrowser *tb_thanks = new Q3TextBrowser(tw_about);
	tb_thanks->setFrameStyle(Q3Frame::NoFrame);
	tb_thanks->setTextFormat(Qt::PlainText);
	tb_thanks->setWordWrap(Q3TextEdit::NoWrap);
	tb_thanks->setText(loadFile("THANKS"));

	// license
	Q3TextBrowser *tb_license = new Q3TextBrowser(tw_about);
	tb_license->setFrameStyle(Q3Frame::NoFrame);
	tb_license->setTextFormat(Qt::PlainText);
	tb_license->setWordWrap(Q3TextEdit::NoWrap);
	tb_license->setText(loadFile("COPYING"));

	// changelog
	Q3TextBrowser *tb_changelog = new Q3TextBrowser(tw_about);
	tb_changelog->setFrameStyle(Q3Frame::NoFrame);
	tb_changelog->setTextFormat(Qt::PlainText);
	tb_changelog->setWordWrap(Q3TextEdit::NoWrap);
	tb_changelog->setText(loadFile("ChangeLog"));

	// add tabs
	tw_about->addTab(tb_authors, tr("A&uthors"));
	tw_about->addTab(tb_thanks, tr("&Thanks"));
	tw_about->addTab(tb_license, tr("&License"));
	tw_about->addTab(tb_changelog, tr("&ChangeLog"));
	// end create our info widgets

	// close button
	Q3HBox *bottom = new Q3HBox(center);
	QWidget *blank2 = new QWidget(bottom);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	QPushButton *pb_close = new QPushButton(/*icons_manager->loadIcon("CloseWindow"),*/ tr("&Close"), bottom/*, "close"*/);
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	// end close button

	layoutHelper->addLabel(l_info);
	loadGeometry(this, "General", "AboutGeometry", 0, 30, 640, 420);

	kdebugf2();
}

About::~About()
{
	kdebugf();

// 	saveGeometry(this, "General", "AboutGeometry");
	delete layoutHelper;

	kdebugf2();
}

void About::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

QString About::loadFile(const QString &name)
{
	kdebugf();

	QFile file(dataPath("kadu/" + name));
	if (!file.open(QIODevice::ReadOnly))
	{
		kdebugm(KDEBUG_ERROR, "About::loadFile(%s) cannot open file\n", name.local8Bit().data());
		return QString::null;
	}

	Q3TextStream str(&file);
	str.setCodec(codec_latin2);
	QString data = str.read();
	file.close();

	kdebugf2();
	return data;
}

void About::resizeEvent(QResizeEvent * /*e*/)
{
	layoutHelper->resizeLabels();
}
