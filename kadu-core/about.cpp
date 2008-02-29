/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>

#include "about.h"
#include "debug.h"
#include "icons_manager.h"
#include "misc.h"

class KaduLink : public QLabel
{
	public:
		KaduLink() : QLabel()
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

About::About(QWidget *parent, const char *name) : QWidget(parent, name, Qt::Window),
	layoutHelper(new LayoutHelper())
{
	kdebugf();

	// set window properties and flags
	setWindowTitle(tr("About"));
	setAttribute(Qt::WA_DeleteOnClose);
	// end set window properties and flags

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget;

	QLabel *l_icon = new QLabel;
	l_icon->setPixmap(icons_manager->loadPixmap("AboutIcon"));

	QWidget *blank = new QWidget;
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBoxLayout *left_layout = new QVBoxLayout;
	left_layout->addWidget(l_icon);
	left_layout->addWidget(blank);

	left->setLayout(left_layout);

	QWidget *center = new QWidget;

	QWidget *texts = new QWidget;

	QLabel *l_info = new QLabel;
	l_info->setText(QString("<span style=\"font-size: 12pt\">Kadu %1 %2<br />(c) 2001-2008 Kadu Team</span>").arg(VERSION)
			.arg(strlen(detailed_version) > 0 ? ("(" + QString(detailed_version) + ")") : QString::null));
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

	QHBoxLayout *texts_layout = new QHBoxLayout;
	texts_layout->addWidget(l_info);
	texts_layout->addWidget(new KaduLink());
	texts->setLayout(texts_layout);

	// end create main QLabel widgets (icon and app info)

	// our TabWidget
	QTabWidget *tw_about = new QTabWidget;
	// end our TabWidget

	// create our info widgets
	// authors
	QTextEdit *tb_authors = new QTextEdit(tw_about);
	tb_authors->setReadOnly(true);
	tb_authors->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	tb_authors->setWordWrapMode(QTextOption::NoWrap);
	tb_authors->setText(loadFile("AUTHORS"));

	// people to thank
	QTextEdit *tb_thanks = new QTextEdit(tw_about);
	tb_thanks->setReadOnly(true);
	tb_thanks->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	tb_thanks->setWordWrapMode(QTextOption::NoWrap);
	tb_thanks->setText(loadFile("THANKS"));

	// license
	QTextEdit *tb_license = new QTextEdit(tw_about);
	tb_license->setReadOnly(true);
	tb_license->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	tb_license->setWordWrapMode(QTextOption::NoWrap);
	tb_license->setText(loadFile("COPYING"));

	// changelog
	QTextEdit *tb_changelog = new QTextEdit(tw_about);
	tb_changelog->setReadOnly(true);
	tb_changelog->setFrameStyle(Q3Frame::QFrame::StyledPanel | QFrame::Raised);
	tb_changelog->setWordWrapMode(QTextOption::NoWrap);
	tb_changelog->setText(loadFile("ChangeLog"));

	// add tabs
	tw_about->addTab(tb_authors, tr("A&uthors"));
	tw_about->addTab(tb_thanks, tr("&Thanks"));
	tw_about->addTab(tb_license, tr("&License"));
	tw_about->addTab(tb_changelog, tr("&ChangeLog"));
	// end create our info widgets

	QVBoxLayout *center_layout = new QVBoxLayout;
	center_layout->addWidget(texts);
	center_layout->addWidget(tw_about);

	center->setLayout(center_layout);

	// close button
	QWidget *bottom = new QWidget;

	QWidget *blank2 = new QWidget;
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"));
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *bottom_layout = new QHBoxLayout;
	bottom_layout->addWidget(blank2);
	bottom_layout->addWidget(pb_close);

	bottom->setLayout(bottom_layout);
	// end close button
	center_layout->addWidget(bottom);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(left);
	layout->addWidget(center);

	setLayout(layout);

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

	QTextStream str(&file);
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
