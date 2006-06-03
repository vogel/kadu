/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"
#include "kadu_splitter.h"
#include "kadu_text_browser.h"
#include "misc.h"

KaduSplitter::KaduSplitter(QWidget* parent, const char* name)
	: QSplitter (parent, name), textbrowsers()
{
}

KaduSplitter::KaduSplitter(Orientation o, QWidget* parent, const char* name)
	: QSplitter(o,parent,name), textbrowsers()
{
}

void KaduSplitter::drawContents(QPainter *p)
{
	QSplitter::drawContents(p);
	kdebugf();
	FOREACH(browser, textbrowsers)
		(*browser)->viewport()->repaint();
//	kdebugf2();
}

void KaduSplitter::childEvent(QChildEvent *c)
{
	QSplitter::childEvent(c);
	kdebugf();
	QObject *o=c->child();
	if (o->inherits("KaduTextBrowser"))
	{
		if (c->inserted())
			textbrowsers.append((KaduTextBrowser*)o);
		else
			textbrowsers.remove((KaduTextBrowser*)o);
	}
//	kdebugm(KDEBUG_INFO, "%d %d %p %p %s %s\n", c->inserted(), c->removed(), this, o, o->className(), o->name());
}
