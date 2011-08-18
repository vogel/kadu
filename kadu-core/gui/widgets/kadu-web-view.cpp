/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2010 Tomasz Rostański (rozteck@interia.pl)
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

/*
 * Copyright for copying and drag'n'drop code from Psi+:
 *
 * Copyright (C) 2010 senu, Rion
 */

#include <QtCore/QEvent>
#include <QtCore/QFile>
#include <QtCore/QMimeData>
#include <QtCore/QPoint>
#include <QtCore/QWeakPointer>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QDrag>
#include <QtGui/QFileDialog>
#include <QtGui/QImage>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtGui/QStyle>
#include <QtGui/QTextDocument>
#include <QtWebKit/QWebHitTestResult>
#include <QtWebKit/QWebHistory>
#include <QtWebKit/QWebPage>

#include "configuration/configuration-file.h"
#include "gui/windows/message-dialog.h"
#include "protocols/services/chat-image-service.h"
#include "url-handlers/url-handler-manager.h"

#include "debug.h"

#include "kadu-web-view.h"

KaduWebView::KaduWebView(QWidget *parent) :
		QWebView(parent), DraggingPossible(false), IsLoading(false), RefreshTimer(new QTimer(this))
{
	kdebugf();

	QWebSettings::setMaximumPagesInCache(0);
	QWebSettings::setObjectCacheCapacities(0, 0, 0);


	setAttribute(Qt::WA_NoBackground);
	setAcceptDrops(false);

	setPage(page());

	connect(RefreshTimer, SIGNAL(timeout()), this, SLOT(reload()));

#ifdef Q_WS_MAEMO_5
	/* Workaround for Qt kinetic scrolling issue in QWebView */
	installEventFilter(this);
#endif

	kdebugf2();
}

KaduWebView::~KaduWebView()
{
}

void KaduWebView::setPage(QWebPage *page)
{
	QWebView::setPage(page);
	page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

	page->history()->setMaximumItemCount(0);

	connect(page, SIGNAL(linkClicked(const QUrl &)), this, SLOT(hyperlinkClicked(const QUrl &)));
	connect(page, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
	connect(page, SIGNAL(loadFinished(bool)), this, SLOT(loadFinishedSlot(bool)));
	connect(page->action(QWebPage::Copy), SIGNAL(triggered()), this, SLOT(textCopied()));
	connect(page->action(QWebPage::DownloadImageToDisk), SIGNAL(triggered()), this, SLOT(saveImage()));
}

void KaduWebView::contextMenuEvent(QContextMenuEvent *e)
{
	if (IsLoading)
		return;

	ContextMenuPos = e->pos();
	const QWebHitTestResult &hitTestContent = page()->currentFrame()->hitTestContent(ContextMenuPos);
	bool isImage = hitTestContent.imageUrl().isValid();
	bool isLink = hitTestContent.linkUrl().isValid();

	QAction *copy = pageAction(QWebPage::Copy);
	copy->setText(tr("Copy"));
	QAction *copyLink = pageAction(QWebPage::CopyLinkToClipboard);
	copyLink->setText(tr("Copy Link"));
	copyLink->setEnabled(isLink);
	QAction *copyImage = pageAction(QWebPage::CopyImageToClipboard);
	copyImage->setText(tr("Copy Image"));
	copyImage->setEnabled(isImage);
	QAction *saveImage = pageAction(QWebPage::DownloadImageToDisk);
	saveImage->setText(tr("Save Image"));
	saveImage->setEnabled(isImage);

	QMenu popupMenu(this);

	popupMenu.addAction(copy);
// 	popupmenu.addSeparator();
	popupMenu.addAction(copyLink);
// 	popupmenu.addAction(pageAction(QWebPage::DownloadLinkToDisk));
	popupMenu.addSeparator();
	popupMenu.addAction(copyImage);
	popupMenu.addAction(saveImage);

 	popupMenu.exec(e->globalPos());
 	kdebugf2();
}

// taken from Psi+'s webkit patch, SVN rev. 2638, and slightly modified
void KaduWebView::mouseMoveEvent(QMouseEvent *e)
{
	if (!DraggingPossible || !(e->buttons() & Qt::LeftButton))
	{
		QWebView::mouseMoveEvent(e);
		return;
	}

	if ((e->pos() - DragStartPosition).manhattanLength() < QApplication::startDragDistance())
		return;

	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData();

	QClipboard *clipboard = QApplication::clipboard();
	QMimeData *originalData = new QMimeData();
	foreach (const QString &format, clipboard->mimeData(QClipboard::Clipboard)->formats())
		originalData->setData(format, clipboard->mimeData(QClipboard::Clipboard)->data(format));
	triggerPageAction(QWebPage::Copy);
	textCopied();

	mimeData->setText(clipboard->mimeData()->text());
	mimeData->setHtml(clipboard->mimeData()->html());
	clipboard->setMimeData(originalData);
	drag->setMimeData(mimeData);

	drag->exec(Qt::CopyAction);
}

// taken from Psi+'s webkit patch, SVN rev. 2638, and slightly modified
void KaduWebView::mousePressEvent(QMouseEvent *e)
{
	if (IsLoading)
		return;

	QWebView::mousePressEvent(e);
	if ((e->buttons() & Qt::LeftButton) && page()->mainFrame()->hitTestContent(e->pos()).isContentSelected())
	{
		QSize cs = page()->mainFrame()->contentsSize();
		QSize vs = page()->viewportSize();
		QSize scrollBarsSize = QSize(cs.height() > vs.height() ? 1 : 0, cs.width() > vs.width() ? 1 : 0) *
				style()->pixelMetric(QStyle::PM_ScrollBarExtent);
		QRect visibleContentsRect = QRect(QPoint(0,0), vs - scrollBarsSize);
		DraggingPossible = visibleContentsRect.contains(e->pos());
		DragStartPosition = e->pos();
	}
	else
		DraggingPossible = false;
}

void KaduWebView::mouseReleaseEvent(QMouseEvent *e)
{
	kdebugf();
	QWebView::mouseReleaseEvent(e);
	DraggingPossible = false;

#ifdef Q_WS_X11
	if (!page()->selectedText().isEmpty())
		convertClipboardHtml(QClipboard::Selection);
#endif
}

#ifdef Q_WS_MAEMO_5
bool KaduWebView::eventFilter(QObject *, QEvent *e)
{
	static bool mousePressed = false;
	switch (e->type())
	{
		case QEvent::MouseButtonPress:
			if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton)
				mousePressed = true;
			break;
		case QEvent::MouseButtonRelease:
			if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton)
				mousePressed = false;
			break;
		case QEvent::MouseMove:
			if (mousePressed)
				return true;
			break;
		default:
			break;
	}
	return false;
}
#endif

void KaduWebView::hyperlinkClicked(const QUrl &anchor) const
{
	UrlHandlerManager::instance()->openUrl(anchor.toEncoded());
}

void KaduWebView::loadStarted()
{
	IsLoading = true;
}

void KaduWebView::loadFinishedSlot(bool success)
{
	Q_UNUSED(success)

	IsLoading = false;
}

void KaduWebView::refreshLater()
{
	RefreshTimer->setSingleShot(true);
	RefreshTimer->start(10);
}

void KaduWebView::saveImage()
{
	kdebugf();

	QUrl imageUrl = page()->currentFrame()->hitTestContent(ContextMenuPos).imageUrl();
	QString imageFullPath = (imageUrl.scheme() == "kaduimg")
			? ChatImageService::imagesPath() + imageUrl.path()
			: imageUrl.toLocalFile();
	if (imageFullPath.isEmpty())
		return;

	QImage image;
	QString fileExt = '.' + imageFullPath.section('.', -1);
	bool formatUnknown = false;
	if (fileExt == "." || fileExt.contains('/'))
	{
		// TODO: we'd better guess the file format and not use QImage
		fileExt = ".png";
		formatUnknown = true;

		if (!image.load(imageFullPath))
		{
			MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Cannot save this image"));
			return;
		}
	}

	QWeakPointer<QFileDialog> fd = new QFileDialog(this);
	fd.data()->setFileMode(QFileDialog::AnyFile);
	fd.data()->setAcceptMode(QFileDialog::AcceptSave);
	fd.data()->setDirectory(config_file.readEntry("Chat", "LastImagePath"));
	fd.data()->setFilter(QString("%1 (*%2)").arg(qApp->translate("ImageDialog", "Images"), fileExt));
	fd.data()->setLabelText(QFileDialog::FileName, imageFullPath.section('/', -1));
	fd.data()->setWindowTitle(tr("Save image"));

	do
	{
		if (fd.data()->exec() != QFileDialog::Accepted)
			break;
		if (fd.data()->selectedFiles().count() < 1)
			break;

		QString file = fd.data()->selectedFiles().at(0);
		if (QFile::exists(file))
		{
			if (MessageDialog::ask(KaduIcon("dialog-question"), tr("Kadu"), tr("File already exists. Overwrite?")))
			{
				QFile removeMe(file);
				if (!removeMe.remove())
				{
					MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Cannot save image: %1").arg(removeMe.errorString()));
					continue;
				}
			}
			else
				continue;
		}

		QString dst = file;
		if (!dst.endsWith(fileExt))
			dst.append(fileExt);

		if (formatUnknown)
		{
			if (!image.save(dst, "PNG"))
			{
				MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Cannot save image"));
				continue;
			}
		}
		else
		{
			QFile src(imageFullPath);
			if (!src.copy(dst))
			{
				MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Cannot save image: %1").arg(src.errorString()));
				continue;
			}
		}

		config_file.writeEntry("Chat", "LastImagePath", fd.data()->directory().absolutePath());
	} while (false);

	delete fd.data();
}

void KaduWebView::textCopied() const
{
	convertClipboardHtml(QClipboard::Clipboard);
}

// taken from Psi+'s webkit patch, SVN rev. 2638, and slightly modified
void KaduWebView::convertClipboardHtml(QClipboard::Mode mode)
{
	// Assume we don't use apostrophes in HTML attributes.

	// Expected string to replace is as follows (capitalics are captured):
	// <img emoticon="1" title="TITLE"*>
	// Source string is created in EmoticonsManager::expandEmoticons().
	static QRegExp emotsRegExp("<img[^>]+emoticon\\s*=\\s*\"1\"[^>]+title\\s*=\\s*\"([^\"]+)\"[^>]*>");

	// Expected string to replace is as follows (capitalics are captured):
	// <a folded="1" displaystr="DISPLAY" href="HREF"*>DISPLAY</a>
	// If first display is different than the second, it means that the user selected only part of the link.
	// Source string is created in StandardUrlHandler::convertUrlsToHtml().
	// BTW, I know it is totally ugly.
	static QRegExp foldedLinksRegExp("<a[^>]+folded\\s*=\\s*\"1\"[^>]+displaystr\\s*=\\s*\"([^\"]+)\"[^>]+href\\s*=\\s*\"([^\"]+)\"[^>]*>([^<]*)<[^>]*>");

	QString html = QApplication::clipboard()->mimeData(mode)->html();

	html.replace(emotsRegExp, QLatin1String("\\1"));

	int pos = 0;
	while (-1 != (pos = foldedLinksRegExp.indexIn(html, pos)))
	{
		int matchedLength = foldedLinksRegExp.matchedLength();
		QString displayStr = foldedLinksRegExp.cap(1);
		QString realDisplayStr = foldedLinksRegExp.cap(3);

		if (displayStr == realDisplayStr) // i.e., we are copying the entire link, not a part of it
		{
			QString hRef = foldedLinksRegExp.cap(2);
			QString unfoldedLink = QString("<a href=\"%1\">%1</a>").arg(hRef);
			html.replace(pos, matchedLength, unfoldedLink);

			pos += unfoldedLink.length();
		}
		else
			pos += matchedLength;
	}

	QTextDocument htmlToPlainTextConverter;
	htmlToPlainTextConverter.setHtml(html);
	QMimeData *data = new QMimeData();
	data->setHtml(html);
	data->setText(htmlToPlainTextConverter.toPlainText());
	QApplication::clipboard()->setMimeData(data, mode);
}

void KaduWebView::setUserFont(const QString &fontString, bool force)
{
	QString style;

	if (fontString.isEmpty())
		style = "* { font-family: sans-serif; }";
	else
	{
		QFont font;
		font.fromString(fontString);
		style = QString("* { %1 }").arg(userFontStyle(font, force));
	}

	QString url = QString("data:text/css;charset=utf-8;base64,%1").arg(QString(style.toUtf8().toBase64()));
	settings()->setUserStyleSheetUrl(url);
}

QString KaduWebView::userFontStyle(const QFont &font, bool force)
{
	QString style = "font-family:" + font.family() + (force ? " !important;" : ";");
	if (force && font.pointSize() != -1)
		style += QString(" font-size:%1pt;").arg(font.pointSize());
	return style;
}
