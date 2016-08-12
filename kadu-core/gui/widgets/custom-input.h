/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtWidgets/QTextEdit>
#include <injeqt/injeqt.h>

#include "chat/chat.h"
#include "gui/widgets/paste-acceptor.h"
#include "exports.h"

class ChatConfigurationHolder;
class Configuration;
class CustomInputMenuManager;
class FormattedString;
class FormattedStringFactory;
class ImageStorageService;
class NormalizedHtmlString;

class KADUAPI CustomInput : public QTextEdit, PasteAcceptor
{
	Q_OBJECT

	QPointer<ChatConfigurationHolder> m_chatConfigurationHolder;
	QPointer<Configuration> m_configuration;
	QPointer<CustomInputMenuManager> m_customInputMenuManager;
	QPointer<ImageStorageService> m_imageStorageService;
	QPointer<FormattedStringFactory> m_formattedStringFactory;

	Chat CurrentChat;

	bool CopyPossible;

	virtual void acceptPlainText(QString plainText) override;
	virtual void acceptFileUrl(QUrl imageUrl) override;
	virtual void acceptImageData(QByteArray imageData) override;

private slots:
	INJEQT_SET void setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setCustomInputMenuManager(CustomInputMenuManager *customInputMenuManager);
	INJEQT_SET void setImageStorageService(ImageStorageService *imageStorageService);
	INJEQT_SET void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);

	void cursorPositionChangedSlot();
	void setCopyPossible(bool available);

protected:
	bool autosend_enabled;

    virtual void showEvent(QShowEvent *e) override;
	virtual void keyPressEvent(QKeyEvent *e) override;
	virtual void keyReleaseEvent(QKeyEvent *e) override;
	virtual void contextMenuEvent(QContextMenuEvent *e) override;
	virtual bool canInsertFromMimeData(const QMimeData *source) const override;
	virtual void insertFromMimeData(const QMimeData *source) override;

public:
	explicit CustomInput(Chat chat, QWidget *parent = nullptr);

	NormalizedHtmlString htmlMessage() const;
	// TODO: to remove
	void setFormattedString(const FormattedString &formattedString);

public slots:
	void setAutoSend(bool on);
	void pasteAndSend();

signals:
	void sendMessage();

	void keyPressed(QKeyEvent *e, CustomInput *sender, bool &handled);
	void keyReleased(QKeyEvent *e, CustomInput *sender, bool &handled);

	void fontChanged(QFont font);

};
