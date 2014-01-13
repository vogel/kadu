/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QPointer>
#include <QtGui/QClipboard>
#include <QtWebKit/QWebView>

#include "exports.h"

class QPoint;
class QTimer;
class QUrl;

class ImageStorageService;

class KADUAPI KaduWebView : public QWebView
{
	Q_OBJECT

	QPointer<ImageStorageService> CurrentImageStorageService;

	bool DraggingPossible;
	bool IsLoading;
	QPoint ContextMenuPos;
	QPoint DragStartPosition;
	QTimer *RefreshTimer;

	static void convertClipboardHtml(QClipboard::Mode mode);

private slots:
	void hyperlinkClicked(const QUrl &anchor) const;
	void loadStarted();
	void loadFinishedSlot(bool success);
	void refreshLater();
	void saveImage();
	void textCopied() const;

#ifdef DEBUG_ENABLED
	void runInspector(bool toggled);
#endif

protected:
	virtual void contextMenuEvent(QContextMenuEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);

public:
	explicit KaduWebView(QWidget *parent = 0);
	virtual ~KaduWebView();

	void setImageStorageService(ImageStorageService *imageStorageService);
	ImageStorageService * imageStorageService() const;

	// hides QWebPage::setPage() (non-virtual)
	void setPage(QWebPage *page);

	void setUserFont(const QString &fontString, bool force);
	QString userFontStyle(const QFont &font, bool force);

};

#endif
