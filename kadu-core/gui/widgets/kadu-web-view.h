/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QPointer>
#include <QtGui/QClipboard>
#include <QtWebKitWidgets/QWebView>
#include <injeqt/injeqt.h>

#include "exports.h"

class QPoint;
class QTimer;
class QUrl;

class ClipboardHtmlTransformerService;
class Configuration;
class IconsManager;
class ImageStorageService;
class UrlHandlerManager;

class KADUAPI KaduWebView : public QWebView
{
	Q_OBJECT

	QPointer<ClipboardHtmlTransformerService> m_clipboardHtmlTransformerService;
	QPointer<Configuration> m_configuration;
	QPointer<IconsManager> m_iconsManager;
	QPointer<ImageStorageService> m_imageStorageService;
	QPointer<UrlHandlerManager> m_urlHandlerManager;

	bool DraggingPossible;
	bool IsLoading;
	QPoint ContextMenuPos;
	QPoint DragStartPosition;
	QTimer *RefreshTimer;

	void convertClipboardHtml(QClipboard::Mode mode) const;

private slots:
	INJEQT_SET void setClipboardHtmlTransformerService(ClipboardHtmlTransformerService *clipboardHtmlTransformerService);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setImageStorageService(ImageStorageService *imageStorageService);
	INJEQT_SET void setUrlHandlerManager(UrlHandlerManager *urlHandlerManager);

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
	Configuration * configuration();

	virtual void contextMenuEvent(QContextMenuEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);

public:
	explicit KaduWebView(QWidget *parent = nullptr);
	virtual ~KaduWebView();

	ImageStorageService * imageStorageService() const;

	void setUserFont(const QString &fontString, bool force);
	QString userFontStyle(const QFont &font, bool force);

};
