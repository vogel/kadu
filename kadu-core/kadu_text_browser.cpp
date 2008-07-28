/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QFile>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QFileDialog>
#include <QtGui/QMenu>
#include <QtGui/QToolTip>

#include "config_file.h"
#include "debug.h"
#include "html_document.h"
#include "message_box.h"
#include "misc.h"

#include "kadu_text_browser.h"

KaduTextBrowser::KaduTextBrowser(QWidget *parent)
	: QWebView(parent),
	refreshTimer(), anchor(), level(0), image()
{
	kdebugf();

// 	setAttribute(Qt::WA_StaticContents);
	setAttribute(Qt::WA_NoBackground);

	setAcceptDrops(false);
// 	viewport()->setAcceptDrops(false);

// 	setOpenLinks(false);
//	setResizePolicy(QScrollView::AutoOne);
	connect(this, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(hyperlinkClicked(const QUrl &)));
	connect(this, SIGNAL(highlighted(const QString&)), this, SLOT(linkHighlighted(const QString &)));
// 	setLineWrapMode(QTextEdit::WidgetWidth/**QTextEdit::AtWordOrDocumentBoundary*/);
// 	setTextFormat(Qt::RichText);

//	connect(verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(repaint()));
//	connect(verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(refresh()));
//	connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(refreshLater()));
	connect(this, SIGNAL(textChanged()), this, SLOT(refreshLater()));
	connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));

//	connect(this, SIGNAL(verticalSliderPressed()), this, SLOT(verticalSliderPressedSlot()));
//	connect(this, SIGNAL(verticalSliderReleased()), this, SLOT(verticalSliderReleasedSlot()));

	kdebugf2();
}

void KaduTextBrowser::verticalSliderPressedSlot()
{
// 	AnimatedLabel::mustPause = true;
// 
// 	const QObjectList *objs = viewport()->children();
// 	if (objs)
// 		CONST_FOREACH(i, *objs)
// 			if ((*i)->inherits("AnimatedLabel"))
// 			{
// 				AnimatedLabel *lab = static_cast<AnimatedLabel *>(*i);
// 				lab->pauseMovie();
// 			}
}

void KaduTextBrowser::verticalSliderReleasedSlot()
{
// 	AnimatedLabel::mustPause = false;
// 
// 	const QObjectList *objs = viewport()->children();
// 	if (objs)
// 		CONST_FOREACH(i, *objs)
// 			if ((*i)->inherits("AnimatedLabel"))
// 			{
// 				AnimatedLabel *lab = static_cast<AnimatedLabel *>(*i);
// 				if (lab->isVisible())
// 				{
//					kdebugm(KDEBUG_INFO, "%s visible\n", lab->tip.local8Bit().data());
// 					lab->unpauseMovie();
// 				}
//				else
//					kdebugm(KDEBUG_INFO, "%s is NOT visible\n", lab->tip.local8Bit().data());
// 			}
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
	//repaintContents(false);
	repaint();
	kdebugf2();
}

void KaduTextBrowser::linkHighlighted(const QString & link)
{
	QToolTip::showText(QCursor::pos(), link);
}

void KaduTextBrowser::setSource(const QString &/*name*/)
{
}

void KaduTextBrowser::setMargin(int width)
{
	setContentsMargins(width, width, width, width);
}

void KaduTextBrowser::contextMenuEvent(QContextMenuEvent * event)
{
//	TODO: 0.6.5
// 	image = imageAt(event->pos());
// 	if (!image.isEmpty())
// 		kdebugm(KDEBUG_INFO, "image: %s\n", image.local8Bit().data());
// 
// 	QMenu *popupmenu = createStandardContextMenu(event->pos());
// 
// 	if (!image.isNull())
// 		popupmenu->addAction(tr("&Save image..."), this, SLOT(saveImage()));
// 	popupmenu->popup(event->globalPos());
// 	kdebugf2();
}

void KaduTextBrowser::hyperlinkClicked(const QUrl &anchor) const
{
	const QString &link = anchor.toString();
	if (link.find(HtmlDocument::urlRegExp()) != -1)
	{
		if (link.startsWith("www."))
			openWebBrowser("http://" + link);
		else
			openWebBrowser(link);
	}
	else if (link.find(HtmlDocument::mailRegExp()) != -1)
		openMailClient(link);
	else if (link.find(HtmlDocument::ggRegExp()) != -1)
		openGGChat(link);
}
/* uncomment if needed
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
*/
void KaduTextBrowser::mouseReleaseEvent(QMouseEvent *e)
{
	kdebugf();
	emit mouseReleased(e);
	QWebView::mouseReleaseEvent(e);
}

void KaduTextBrowser::wheelEvent(QWheelEvent *e)
{
	kdebugf();
	emit wheel(e);
	QWebView::wheelEvent(e);
}

QString KaduTextBrowser::imageAt(const QPoint &point)
{/*
	// this function uses Qt private API (QTextCursor), because there's no way
	// to do it with public API (strange bugs in Qt :()
	kdebugf();
//	kdebugm(KDEBUG_INFO, "point.y()=%d  contentsHeight()=%d\n", point.y(), contentsHeight());
	if (point.y() > contentsHeight()) // certainly no image - it prevents invalid results
		return QString::null;
	static QRegExp imgExp("^<!--StartFragment-->(<.+>)?(/.*\\.\\w{3,4})$");
	bool ok = false;
// 	Q3TextCursor *c = textCursor();
// 	QTextDocument *doc = c->document();

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
	kdebugf2();*/
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
	fd->setLabelText(QFileDialog::FileName, image.section('/', -1));
	fd->setWindowTitle(tr("Save image"));
	while ((fdResult = fd->exec()) == QFileDialog::Accepted
		&& QFile::exists(fd->selectedFile())
		&& !MessageBox::ask(tr("File already exists. Overwrite?")));
	if (fdResult == QFileDialog::Accepted)
	{
		QFile dst((fd->selectedFile().endsWith(fileExt)) ? fd->selectedFile() : fd->selectedFile() + fileExt);
		QFile src(image);
		if (dst.open(QIODevice::WriteOnly))
		{
			if (src.open(QIODevice::ReadOnly))
			{
				char buffer[1024];
				Q_LONG len;
				while (!src.atEnd() && (len = src.readBlock(buffer, sizeof(buffer))) > 0
					&& dst.writeBlock(buffer, len) != -1);
				src.close();
			}
			config_file.writeEntry("Chat", "LastImagePath", fd->directory().absolutePath());
			dst.close();
		}
		else
			MessageBox::msg(tr("Cannot save image: %1").arg(dst.errorString()), false, "Warning");
	}
	delete fd;
	kdebugf2();
}

