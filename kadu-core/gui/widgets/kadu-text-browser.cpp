/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QMenu>
#include <QtGui/QToolTip>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebHitTestResult>

#include "configuration/configuration-file.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "url-handlers/url-handler-manager.h"

#include "debug.h"
#include "html_document.h"

#include "kadu-text-browser.h"

KaduTextBrowser::KaduTextBrowser(QWidget *parent)
	: QWebView(parent), refreshTimer()
{
	kdebugf();

	setAttribute(Qt::WA_NoBackground);
	setAcceptDrops(false);

	setPage(page());

	connect(pageAction(QWebPage::DownloadImageToDisk), SIGNAL(triggered()), this, SLOT(saveImage()));
	connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(reload()));

	kdebugf2();
}

void KaduTextBrowser::setPage(QWebPage * page)
{
	QWebView::setPage(page);
	page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

	connect(page, SIGNAL(linkClicked(const QUrl &)), this, SLOT(hyperlinkClicked(const QUrl &)));
	connect(page, SIGNAL(linkHovered(const QString&,  const QString&, const QString&)), this, SLOT(linkHighlighted(const QString &)));
}

void KaduTextBrowser::refreshLater()
{
	refreshTimer.setSingleShot(true);
	refreshTimer.start(10);
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

void KaduTextBrowser::contextMenuEvent(QContextMenuEvent *event)
{
	contextMenuPos = event->pos();
	const QWebHitTestResult &hitTestContent = page()->currentFrame()->hitTestContent(contextMenuPos);
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

	QMenu *popupmenu = new QMenu();

	popupmenu->addAction(copy);
// 	popupmenu->addSeparator();
	popupmenu->addAction(copyLink);
// 	popupmenu->addAction(pageAction(QWebPage::DownloadLinkToDisk));
	popupmenu->addSeparator();
	popupmenu->addAction(copyImage);
	popupmenu->addAction(saveImage);

 	popupmenu->popup(event->globalPos());
 	kdebugf2();
}

void KaduTextBrowser::hyperlinkClicked(const QUrl &anchor) const
{
	UrlHandlerManager::instance()->openUrl(anchor.toString());
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

	QString image = page()->currentFrame()->hitTestContent(contextMenuPos).imageUrl().toLocalFile();
	if (image.isEmpty())
		return;

	QString fileExt = '.' + image.section('.', -1);

	QFileDialog fd(this);
	fd.setFileMode(QFileDialog::AnyFile);
	fd.setDirectory(config_file.readEntry("Chat", "LastImagePath"));
	fd.setFilter(QString("%1 (*%2)").arg(qApp->translate("ImageDialog", "Images"), fileExt));
	fd.setLabelText(QFileDialog::FileName, image.section('/', -1));
	fd.setWindowTitle(tr("Save image"));

	while (true)
	{
		if (fd.exec() != QFileDialog::Accepted)
			break;
		if (fd.selectedFiles().count() < 1)
			break;

		QString file = fd.selectedFiles()[0];
		if (QFile::exists(file))
		{
			if (MessageDialog::ask(tr("File already exists. Overwrite?")))
			{
				QFile removeMe(file);
				if (!removeMe.remove())
				{
					MessageDialog::msg(tr("Cannot save image: %1").arg(removeMe.errorString()), false, "32x32/dialog-warning.png");
					continue;
				}
			}
			else
				continue;
		}

		QString dst = file;
		if (!dst.endsWith(fileExt))
			dst.append(fileExt);

		QFile src(image);
		if (!src.copy(dst))
		{
			MessageDialog::msg(tr("Cannot save image: %1").arg(src.errorString()), false, "32x32/dialog-warning.png");
			continue;
		}

		config_file.writeEntry("Chat", "LastImagePath", fd.directory().absolutePath());
		break;
	}
}
