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

#ifndef KADU_CUSTOM_INPUT_H
#define KADU_CUSTOM_INPUT_H

#include <QtCore/QPointer>
#include <QtWidgets/QTextEdit>

#include "chat/chat.h"

#include "exports.h"

class FormattedString;
class FormattedStringFactory;
class ImageStorageService;

class KADUAPI CustomInput : public QTextEdit
{
	Q_OBJECT

	QPointer<ImageStorageService> CurrentImageStorageService;
	QPointer<FormattedStringFactory> CurrentFormattedStringFactory;

	Chat CurrentChat;

	bool CopyPossible;

private slots:
	void cursorPositionChangedSlot();
	void setCopyPossible(bool available);

protected:
	bool autosend_enabled;

    virtual void showEvent(QShowEvent *e) override;
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);
	virtual void contextMenuEvent(QContextMenuEvent *e);
	virtual bool canInsertFromMimeData(const QMimeData *source) const;
	virtual void insertFromMimeData(const QMimeData *source);

public:
	explicit CustomInput(Chat chat, QWidget *parent = 0);

	void setImageStorageService(ImageStorageService *imageStorageService);
	void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);

	void setFormattedString(const FormattedString &formattedString);
	std::unique_ptr<FormattedString> formattedString() const;

public slots:
	void setAutoSend(bool on);
	void pasteAndSend();

signals:
	void sendMessage();

	void keyPressed(QKeyEvent *e, CustomInput *sender, bool &handled);
	void keyReleased(QKeyEvent *e, CustomInput *sender, bool &handled);

	void fontChanged(QFont font);

};

#endif
