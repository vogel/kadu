/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qclipboard.h>
#include <qpopupmenu.h>
#include <qregexp.h>

#include "debug.h"
#include "kadu_text_browser.h"
#include "misc.h"

KaduTextBrowser::KaduTextBrowser(QWidget *parent, const char *name)
	: QTextBrowser(parent, name),QToolTip(viewport()),level(0)
{
	kdebugf();

	setAcceptDrops(false);
	viewport()->setAcceptDrops(false);

//	setResizePolicy(QScrollView::AutoOne);
//	setWFlags(Qt::WNoAutoErase|Qt::WStaticContents|Qt::WPaintClever);
	connect(this, SIGNAL(linkClicked(const QString&)), this, SLOT(hyperlinkClicked(const QString&)));
	connect(this, SIGNAL(highlighted(const QString&)), this, SLOT(linkHighlighted(const QString &)));
	setWrapPolicy(QTextEdit::AtWordOrDocumentBoundary);
	setTextFormat(Qt::RichText);

//	connect(verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(repaint()));
	kdebugf2();
}

void KaduTextBrowser::maybeTip(const QPoint &c)
{
	if (!highlightedlink.isNull())
		kdebugmf(KDEBUG_INFO, "link %s (X,Y)=%d,%d\n", highlightedlink.local8Bit().data(), c.x(), c.y());
	tip(QRect(c.x()-20,c.y()-5,40,10), highlightedlink);
}

void KaduTextBrowser::linkHighlighted(const QString & link)
{
	highlightedlink = link;
}

void KaduTextBrowser::setSource(const QString &/*name*/)
{
}

void KaduTextBrowser::setMargin(int width)
{
	setMargins(width, width, width, width);
}

void KaduTextBrowser::copyLinkLocation()
{
	kdebugmf(KDEBUG_FUNCTION_START, "anchor = %s\n", anchor.local8Bit().data());
	QApplication::clipboard()->setText(anchor);
}

QPopupMenu *KaduTextBrowser::createPopupMenu(const QPoint &point)
{
	kdebugf();
	anchor = anchorAt(point);
	anchor.replace("%2520", "%20");//obej¶cie b³êdu w Qt, patrz HtmlDocument::convertUrlsToHtml()

	QPopupMenu* popupmenu = QTextBrowser::createPopupMenu(point);

	if (!anchor.isEmpty())
		popupmenu->insertItem(tr("Copy link &location"), this, SLOT(copyLinkLocation()), CTRL+Key_L, -1, 0);

	kdebugf2();
	return popupmenu;
}

void KaduTextBrowser::drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph)
{
	/*
		z nie do koñca wiadomych przyczyn, Qt czasami wpada w pêtle i drawContents
		jeszcze	raz wywo³uje sam± siebie, co powoduje wypisanie:
			QPixmap::operator=: Cannot assign to pixmap during painting
			QPaintDevice: Cannot destroy paint device that is being painted
		oraz zawieszenie Kadu (http://www.kadu.net/forum/viewtopic.php?t=2486)
	*/
//	kdebugmf(KDEBUG_INFO, "level: %d\n", level);
	++level;
	if (level==1)
	{
//		kdebugm(KDEBUG_INFO, "x:%d y:%d w:%d h:%d\n", clipx, clipy, clipw, cliph);
		QTextBrowser::drawContents(p, clipx, clipy, clipw, cliph);
//		QTimer::singleShot(0, this, SLOT(repaint()));//niestety konieczne
	}
	--level;
}

void KaduTextBrowser::hyperlinkClicked(const QString &link) const
{
	if (link.startsWith("www."))
		openWebBrowser("http://"+link);
	else
		openWebBrowser(link);
}

void KaduTextBrowser::copy()
{
	kdebugf();

	int paraFrom, indexFrom, paraTo, indexTo;
	getSelection(&paraFrom, &indexFrom, &paraTo, &indexTo);
//	kdebugm(KDEBUG_DUMP, "selection: %d %d %d %d\n", paraFrom, indexFrom, paraTo, indexTo);
	if (paraFrom==paraTo && indexFrom==indexTo)
		return;

	QString txt=selectedText();
//	kdebugm(KDEBUG_DUMP, "%d    plain:%d rich:%d auto:%d log:%d\n", textFormat(), Qt::PlainText, Qt::RichText, Qt::AutoText, Qt::LogText);
//	kdebugm(KDEBUG_DUMP, "\n%s\n----------------------\n", txt.local8Bit().data());

	//czasem siê to cholerstwo pojawia gdy dostajemy ca³y dokument (bug w qt!),
	//wiêc wypada³oby pozbyæ siê wszystkich zbêdnych tagów...
	txt.replace("<html>", "");
	txt.replace("</html>", "");

	txt.replace("<head>", "");
	txt.replace("</head>", "");

	txt.replace(QRegExp("<meta[^>]+>"), "");

	txt.replace("<body>", "");
	txt.replace(QRegExp("<body [^>]+>"), "");
	txt.replace("</body>", "");

	txt.replace(QRegExp("<a [^>]+>"), "");
	txt.replace("</a>", "");

	txt.replace("<br>", "\n");
	txt.replace("<br/>", "\n");
	txt.replace("<br />", "\n");

	//usuwamy wszystkie znane tagi htmla, które mog± siê pojawiæ w chacie
	//nie mo¿na u¿yæ po prostu <[^>]+>, bo za³api± siê te¿ emotikony typu <rotfl>
	txt.replace(QRegExp("<![^>]+>"), "");//<!--StartFragment-->

	txt.replace("<p>", "");
	txt.replace(QRegExp("<p [^>]+>"), "");
	txt.replace("</p>", "");

	txt.replace("<span>", "");
	txt.replace(QRegExp("<span [^>]+>"), "");
	txt.replace("</span>", "");

	txt.replace("<table>", "");
	txt.replace(QRegExp("<table [^>]+>"), "");
	txt.replace("</table>", "");

	txt.replace("<tr>", "");
	txt.replace(QRegExp("<tr [^>]+>"), "");
	txt.replace("</tr>", "");

	txt.replace("<td>", "");
	txt.replace(QRegExp("<td [^>]+>"), "");
	txt.replace("</td>", "");

	//specjalnie traktujemy obrazki, mo¿e u¿ytkownik domy¶li siê o co tu chodzi :P
	txt.replace(QRegExp("<img gg_crc=([0-9]*) gg_sender=([0-9]*) gg_size=([0-9]*) src=[^>]+>"), "\\2-\\3-\\1-*");
	txt.replace(QRegExp("<img src=([^>]+)>"), "\\1");

//	txt.replace(QRegExp("<[^>]+>[^<]+</[^>]+>"), "");

	txt.replace("&lt;", "<");
	txt.replace("&gt;", ">");
	txt.replace("&amp;", "&");
	txt.replace("&quot;", "\"");

//	kdebugm(KDEBUG_DUMP, "result: \n%s\n\n", txt.local8Bit().data());

	QClipboard *clipboard=QApplication::clipboard();
	clipboard->setText(txt, QClipboard::Clipboard);
	clipboard->setText(txt, QClipboard::Selection);
	kdebugf2();
}

void KaduTextBrowser::contentsMouseReleaseEvent(QMouseEvent *e)
{
	kdebugf();
	emit mouseReleased(e, this);
	QTextBrowser::contentsMouseReleaseEvent(e);
}
