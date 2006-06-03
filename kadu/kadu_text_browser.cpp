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
#include <qdialog.h>
#include <qfile.h>
#include <private/qrichtext_p.h>

#include "debug.h"
#include "kadu_text_browser.h"
#include "misc.h"
#include "config_dialog.h"
#include "message_box.h"

KaduTextBrowser::KaduTextBrowser(QWidget *parent, const char *name)
	: QTextBrowser(parent, name), QToolTip(viewport()),
	refreshTimer(), anchor(), level(0), highlightedlink(), image()
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
//	connect(verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(refresh()));
	connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(refreshLater()));
	connect(this, SIGNAL(textChanged()), this, SLOT(refreshLater()));
	connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));
	kdebugf2();
}

void KaduTextBrowser::refreshLater()
{
	refreshTimer.start(10, true);
}

void KaduTextBrowser::refresh()
{
	kdebugf();
	//sync();
	//repaint();
	repaintContents(false);
}

void KaduTextBrowser::maybeTip(const QPoint &c)
{
	if (!highlightedlink.isEmpty())
		kdebugmf(KDEBUG_INFO, "link %s (X,Y)=%d,%d\n", highlightedlink.local8Bit().data(), c.x(), c.y());
	tip(QRect(c.x() - 20, c.y() - 5, 40, 10), highlightedlink);
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
	anchor.replace("%2520", "%20");//workaround for bug in Opera, see: HtmlDocument::convertUrlsToHtml()
	image = imageAt(point);
	if (!image.isEmpty())
		kdebugm(KDEBUG_INFO, "image: %s\n", image.local8Bit().data());

	QPopupMenu *popupmenu = QTextBrowser::createPopupMenu(point);

	if (!anchor.isEmpty())
		popupmenu->insertItem(tr("Copy link &location"), this, SLOT(copyLinkLocation()), CTRL+Key_L, -1, 0);
	else if (!image.isNull())
		popupmenu->insertItem(tr("&Save image..."), this, SLOT(saveImage()));

	kdebugf2();
	return popupmenu;
}

void KaduTextBrowser::drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph)
{
	/*
		for unknown reasons, QTextBrowser::drawContents() sometimes invokes itself, which produces:
			QPixmap::operator=: Cannot assign to pixmap during painting
			QPaintDevice: Cannot destroy paint device that is being painted
		and freeze of application (http://www.kadu.net/forum/viewtopic.php?t=2486)
	*/
//	kdebugmf(KDEBUG_INFO, "level: %d\n", level);
	++level;
	if (level == 1)
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
		openWebBrowser("http://" + link);
	else
		openWebBrowser(link);
}

void KaduTextBrowser::copy()
{
	kdebugf();

	int paraFrom, indexFrom, paraTo, indexTo;
	getSelection(&paraFrom, &indexFrom, &paraTo, &indexTo);
//	kdebugm(KDEBUG_DUMP, "selection: %d %d %d %d\n", paraFrom, indexFrom, paraTo, indexTo);
	if (paraFrom == paraTo && indexFrom == indexTo)
		return;

	QString txt = selectedText();
//	kdebugm(KDEBUG_DUMP, "%d    plain:%d rich:%d auto:%d log:%d\n", textFormat(), Qt::PlainText, Qt::RichText, Qt::AutoText, Qt::LogText);
//	kdebugm(KDEBUG_DUMP, "\n%s\n----------------------\n", txt.local8Bit().data());

	// sometimes we get all document (bug in Qt!)
	// so it would be nice to remove all these tags ;)
	txt.remove("<html>");
	txt.remove("</html>");

	txt.remove("<head>");
	txt.remove("</head>");

	txt.remove(QRegExp("<meta[^>]+>"));

	txt.remove("<body>");
	txt.remove(QRegExp("<body [^>]+>"));
	txt.remove("</body>");

	txt.remove(QRegExp("<a [^>]+>"));
	txt.remove("</a>");

	txt.replace("<br>", "\n");
	txt.replace("<br/>", "\n");
	txt.replace("<br />", "\n");

	// remove all known html tags, which can show up in Chat
	// we cannot use <[^>]+> because it would catch emoticons like <rotfl>
	txt.remove(QRegExp("<![^>]+>"));//<!--StartFragment-->

	txt.remove("<p>");
	txt.remove(QRegExp("<p [^>]+>"));
	txt.remove("</p>");

	txt.remove("<span>");
	txt.remove(QRegExp("<span [^>]+>"));
	txt.remove("</span>");

	txt.remove("<table>");
	txt.remove(QRegExp("<table [^>]+>"));
	txt.remove("</table>");

	txt.remove("<tr>");
	txt.remove(QRegExp("<tr [^>]+>"));
	txt.remove("</tr>");

	txt.remove("<td>");
	txt.remove(QRegExp("<td [^>]+>"));
	txt.remove("</td>");

	txt.remove("<hr>");

	// treat images special
	txt.replace(QRegExp("<img gg_crc=([0-9]*) gg_sender=([0-9]*) gg_size=([0-9]*) src=[^>]+>"), "\\2-\\3-\\1-*");
	txt.replace(QRegExp("<img src=([^>]+)>"), "\\1");

//	txt.remove(QRegExp("<[^>]+>[^<]+</[^>]+>"));

	txt.replace("&lt;", "<");
	txt.replace("&gt;", ">");
	txt.replace("&amp;", "&");
	txt.replace("&quot;", "\"");

//	kdebugm(KDEBUG_DUMP, "result: \n%s\n\n", txt.local8Bit().data());

	QClipboard *clipboard = QApplication::clipboard();
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

QString KaduTextBrowser::imageAt(const QPoint &point)
{
	// this function uses Qt private API (QTextCursor), because there's no way
	// to do it with public API (strange bugs in Qt :()
	kdebugf();
//	kdebugm(KDEBUG_INFO, "point.y()=%d  contentsHeight()=%d\n", point.y(), contentsHeight());
	if (point.y() > contentsHeight()) // certainly no image - it prevents invalid results
		return QString::null;
	static QRegExp imgExp("^<!--StartFragment-->(<.+>)?(/.*\\.\\w{3,4})$");
	bool ok = false;
	QTextCursor *c = textCursor();
	QTextDocument *doc = c->document();

	c->place(point, doc->firstParagraph()); // places cursor _near_ specified point
	if (c->paragraph())
	{
//		kdebugm(KDEBUG_INFO, "point.x()=%d c->x()=%d\n", point.x(), c->x());
		// if cursor is set _after_ image
		if (point.x() < c->x())
		{
//			kdebugm(KDEBUG_INFO, "point.x() < c->x()\n");
			QTextCursor endCur(*c);
			c->gotoPreviousLetter();
//			kdebugm(KDEBUG_INFO, "point.x()=%d c->x()=%d\n", point.x(), c->x());
			if (ok = (c->x() < point.x()))
			{
//				kdebugm(KDEBUG_INFO, "c0->x() < point.x()\n");
				doc->setSelectionStart(1, *c);
				doc->setSelectionEnd(1, endCur);
			}
		}
		else // cursor is set _on_ or __before__ image
		{
//			kdebugm(KDEBUG_INFO, "point.x() >= c->x()\n");
			doc->setSelectionStart(1, *c);
			int tries = 0;
			do
			{
				c->gotoNextLetter();
//				kdebugm(KDEBUG_INFO, "point.x()=%d c->x()=%d\n", point.x(), c->x());
				if (ok = (point.x() < c->x()))
				{
//					kdebugm(KDEBUG_INFO, "point.x() < c2->x()\n");
					doc->setSelectionEnd(1, *c);
				}
				++tries;
			} while (!ok && tries < 2);
		}
//		kdebugm(KDEBUG_INFO, "ok: %d\n", ok);
		if (ok && imgExp.search(doc->selectedText (1, true)) != -1)
		{
			removeSelection(1);
			kdebugf2();
			return imgExp.cap(2);
		}
		removeSelection(1);
	}
	kdebugf2();
	return QString::null;
}

void KaduTextBrowser::saveImage()
{
	kdebugf();
	QFileDialog *fd = new QFileDialog(this);
	int fdResult;
	QString fileExt = '.' + image.section('.', -1);

	fd->setMode(QFileDialog::AnyFile);
	fd->setDir(config_file.readEntry("Chat", "LastImagePath"));
	fd->setFilter(QString("%1 (*%2)").arg(qApp->translate("ImageDialog", "Images"), fileExt));
	fd->setSelection(image.section('/', -1));
	fd->setCaption(tr("Save image"));
	while ((fdResult = fd->exec()) == QFileDialog::Accepted
		&& QFile::exists(fd->selectedFile())
		&& !MessageBox::ask(tr("File already exists. Overwrite?")));
	if (fdResult == QFileDialog::Accepted)
	{
		QFile dst((fd->selectedFile().endsWith(fileExt)) ? fd->selectedFile() : fd->selectedFile() + fileExt);
		QFile src(image);
		if (dst.open(IO_WriteOnly))
		{
			if (src.open(IO_ReadOnly))
			{
				char buffer[1024];
				Q_LONG len;
				while (!src.atEnd() && (len = src.readBlock(buffer, sizeof(buffer))) > 0
					&& dst.writeBlock(buffer, len) != -1);
				src.close();
			}
			config_file.writeEntry("Chat", "LastImagePath", fd->dirPath());
			dst.close();
		}
		else
			MessageBox::wrn(tr("Cannot save image: %1").arg(dst.errorString()));
	}
	delete fd;
	kdebugf2();
}
