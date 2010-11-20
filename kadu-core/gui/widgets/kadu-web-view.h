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

#ifndef KADU_TEXT_BROWSER_H
#define KADU_TEXT_BROWSER_H

#include <QtGui/QClipboard>
#include <QtWebKit/QWebView>

#include "exports.h"

class QPoint;
class QTimer;
class QUrl;

class KADUAPI KaduWebView : public QWebView
{
	Q_OBJECT

	bool DraggingPossible;
	bool IsLoading;
	QPoint ContextMenuPos;
	QPoint DragStartPosition;
	QTimer *RefreshTimer;

	static void convertClipboardHtmlImages(QClipboard::Mode mode);

private slots:
	void hyperlinkClicked(const QUrl &anchor) const;
	void loadStarted();
	void loadFinishedSlot(bool success);
	void refreshLater();
	void saveImage();
	void textCopied() const;

protected:
	virtual void contextMenuEvent(QContextMenuEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);

#ifdef Q_WS_MAEMO_5
	virtual bool eventFilter(QObject *, QEvent *e);
#endif

public:
	explicit KaduWebView(QWidget *parent = 0);
	virtual ~KaduWebView();

	// hides QWebPage::setPage() (non-virtual)
	void setPage(QWebPage *page);

};

#endif
