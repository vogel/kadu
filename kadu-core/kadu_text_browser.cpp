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
	refreshTimer(), image()
{
	kdebugf();

// 	setAttribute(Qt::WA_StaticContents);
	setAttribute(Qt::WA_NoBackground);

	setAcceptDrops(false);
// 	viewport()->setAcceptDrops(false);

 	page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
//	setResizePolicy(QScrollView::AutoOne);
	connect(page(), SIGNAL(linkClicked(const QUrl &)), this, SLOT(hyperlinkClicked(const QUrl &)));
	connect(page(), SIGNAL(linkHovered(const QString&,  const QString&, const QString&)), this, SLOT(linkHighlighted(const QString &)));
// 	setLineWrapMode(QTextEdit::WidgetWidth/**QTextEdit::AtWordOrDocumentBoundary*/);
// 	setTextFormat(Qt::RichText);

//	connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(refreshLater()));
//	connect(this, SIGNAL(textChanged()), this, SLOT(refreshLater()));
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
	//repaintContents(false);
	reload();
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
	QMenu *popupmenu = new QMenu;
	popupmenu->addAction(pageAction(QWebPage::CopyLinkToClipboard));
	popupmenu->addAction(pageAction(QWebPage::DownloadImageToDisk));
	popupmenu->addAction(pageAction(QWebPage::CopyImageToClipboard));
	popupmenu->addAction(pageAction(QWebPage::Copy));

 	popupmenu->popup(event->globalPos());
 	kdebugf2();
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

