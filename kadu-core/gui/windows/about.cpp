/*
 * %kadu copyright begin%
 * Copyright 2006, 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>

#include "debug.h"
#include "kadu-config.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "about.h"

class KaduLink : public QLabel
{
protected:
	virtual void mousePressEvent(QMouseEvent *)
	{
		openWebBrowser("http://www.kadu.net/");
	}

public:
	KaduLink() : QLabel()
	{
		setText("<a href=\"http://www.kadu.net/\">www.kadu.net</a>");
		setCursor(QCursor(Qt::PointingHandCursor));
		setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
	}

};

About::About(QWidget *parent)
	: QWidget(parent, Qt::Window)
{
	kdebugf();

	// set window properties and flags
	setWindowTitle(tr("About"));
	setAttribute(Qt::WA_DeleteOnClose);
	// end set window properties and flags

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget;

	QLabel *l_icon = new QLabel;
	l_icon->setPixmap(IconsManager::instance()->pixmapByPath("kadu_icons/kadu.png"));

	QWidget *blank = new QWidget;
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBoxLayout *left_layout = new QVBoxLayout(left);
	left_layout->addWidget(l_icon);
	left_layout->addWidget(blank);

	QWidget *center = new QWidget;

	QWidget *texts = new QWidget;

	QLabel *l_info = new QLabel;
	l_info->setText(QString("<span style=\"font-size: 12pt\">Kadu %1 %2<br />(c) 2001-2010 Kadu Team</span><br><span style=\"font-size: 10pt\">Qt: %3</span>").arg(VERSION)
			.arg(strlen(DETAILED_VERSION) > 0 ? ("(" + QString(DETAILED_VERSION) + ")") : QString::null).arg(qVersion()));
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

	QHBoxLayout *texts_layout = new QHBoxLayout(texts);
	texts_layout->addWidget(l_info);
	texts_layout->addWidget(new KaduLink());
	// end create main QLabel widgets (icon and app info)

	// our TabWidget
	QTabWidget *tw_about = new QTabWidget(this);
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
	tb_changelog->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	tb_changelog->setWordWrapMode(QTextOption::NoWrap);
	tb_changelog->setText(loadFile("ChangeLog"));

	// add tabs
	tw_about->addTab(tb_authors, tr("A&uthors"));
	tw_about->addTab(tb_thanks, tr("&Thanks"));
	tw_about->addTab(tb_license, tr("&License"));
	tw_about->addTab(tb_changelog, tr("&ChangeLog"));
	// end create our info widgets

	QVBoxLayout *center_layout = new QVBoxLayout(center);
	center_layout->addWidget(texts);
	center_layout->addWidget(tw_about);
	// close button
	QWidget *bottom = new QWidget;

	QWidget *blank2 = new QWidget;
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	QPushButton *pb_close = new QPushButton(IconsManager::instance()->iconByPath("16x16/dialog-cancel.png"), tr("&Close"));
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *bottom_layout = new QHBoxLayout(bottom);
	bottom_layout->addWidget(blank2);
	bottom_layout->addWidget(pb_close);

	// end close button
	center_layout->addWidget(bottom);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(left);
	layout->addWidget(center);

	loadWindowGeometry(this, "General", "AboutGeometry", 0, 50, 640, 420);

	kdebugf2();
}

About::~About()
{
	kdebugf();

 	saveWindowGeometry(this, "General", "AboutGeometry");

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
		kdebugm(KDEBUG_ERROR, "About::loadFile(%s) cannot open file\n", qPrintable(name));
		return QString::null;
	}

	QTextStream str(&file);
	str.setCodec(codec_latin2);
	QString data = str.readAll();
	file.close();

	kdebugf2();
	return data;
}
