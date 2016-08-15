/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Slawomir Stepien (s.stepien@interia.pl)
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

#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMimeData>
#include <QtCore/QScopedPointer>
#include <QtCore/QUrl>
#include <QtGui/QImageReader>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>

#include "core/core.h"
#include "formatted-string/formatted-string-factory.h"
#include "formatted-string/formatted-string-html-visitor.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "gui/hot-key.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "html/normalized-html-string.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-image-service.h"
#include "services/image-storage-service.h"
#include "debug.h"

#include "custom-input-menu-manager.h"
#include "custom-input.h"

CustomInput::CustomInput(Chat chat, QWidget *parent) :
		QTextEdit(parent), CurrentChat(chat), CopyPossible(false), autosend_enabled(true)
{
	kdebugf();

	setAcceptRichText(false);

	setAcceptDrops(true);

	connect(this, SIGNAL(copyAvailable(bool)), this, SLOT(setCopyPossible(bool)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChangedSlot()));

	kdebugf2();
}

void CustomInput::setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder)
{
	m_chatConfigurationHolder = chatConfigurationHolder;
}

void CustomInput::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void CustomInput::setCustomInputMenuManager(CustomInputMenuManager *customInputMenuManager)
{
	m_customInputMenuManager = customInputMenuManager;
}

void CustomInput::setImageStorageService(ImageStorageService *imageStorageService)
{
	m_imageStorageService = imageStorageService;
}

void CustomInput::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	m_formattedStringFactory = formattedStringFactory;
}

NormalizedHtmlString CustomInput::htmlMessage() const
{
	auto formattedString = m_formattedStringFactory->fromTextDocument(*document());
	FormattedStringHtmlVisitor visitor{};
	formattedString->accept(&visitor);
	return normalizeHtml(visitor.result());
}

void CustomInput::setFormattedString(const FormattedString &formattedString)
{
	FormattedStringHtmlVisitor html{};
	formattedString.accept(&html);
	setHtml(html.result().string());
}

void CustomInput::showEvent(QShowEvent *e)
{
	QTextEdit::showEvent(e);

	// see #2837 - windows bug
	setFont(m_chatConfigurationHolder->chatFont());
}

void CustomInput::keyPressEvent(QKeyEvent *e)
{
	kdebugf();

	/* Ctrl+Return and Ctrl+Enter have a special meaning:
	 * 1) autosend_enabled -> new line is entered
	 * 2) message is sent
	 */
	bool isCtrlEnter = (e->key() == Qt::Key_Return && e->modifiers() == Qt::ControlModifier)
			|| (e->key() == Qt::Key_Enter && e->modifiers() == (Qt::KeypadModifier | Qt::ControlModifier));

	if ((autosend_enabled
			&& ((e->key() == Qt::Key_Return && e->modifiers() == Qt::NoModifier)
				|| (e->key() == Qt::Key_Enter && e->modifiers() == Qt::KeypadModifier)))
		|| (!autosend_enabled && isCtrlEnter) || HotKey::shortCut(m_configuration, e, "ShortCuts", "chat_sendmessage"))
	{
		kdebugmf(KDEBUG_INFO, "emit sendMessage()\n");
		emit sendMessage();
		e->accept();
		kdebugf2();
		return;
	}
	else if (isCtrlEnter)
	{
		// now surely autosend_enabled == true, so we can emulate Shift+Return to get a new line
		QKeyEvent emulateNewLineEvent(QEvent::KeyPress, Qt::Key_Return, Qt::ShiftModifier, "\n", false, 2);
		QTextEdit::keyPressEvent(&emulateNewLineEvent);

		e->accept();
		kdebugf2();
		return;
	}
	else if (HotKey::shortCut(m_configuration, e, "ShortCuts", "chat_bold"))
	{
		if (QFont::Normal == fontWeight())
			setFontWeight(QFont::Bold);
		else
			setFontWeight(QFont::Normal);

		emit fontChanged(currentFont());

		e->accept();
		kdebugf2();
		return;
	}
	else if (HotKey::shortCut(m_configuration, e, "ShortCuts", "chat_italic"))
	{
		setFontItalic(!fontItalic());

		emit fontChanged(currentFont());

		e->accept();
		kdebugf2();
		return;
	}
	else if (HotKey::shortCut(m_configuration, e, "ShortCuts", "chat_underline"))
	{
		setFontUnderline(!fontUnderline());

		emit fontChanged(currentFont());

		e->accept();
		kdebugf2();
		return;
	}
	else if (e->matches(QKeySequence::SelectAll))
	{
		selectAll();
		e->accept();
		kdebugf2();
		return;
	}
	else if (CopyPossible && e->matches(QKeySequence::Copy))
	{
		copy();
		e->accept();
		kdebugf2();
		return;
	}
	else if (e->key() == Qt::Key_Tab)
	{
		e->accept();
		return;
	}

	bool handled = false;
	emit keyPressed(e, this, handled);
	if (handled)
	{
		e->accept();
		kdebugf2();
		return;
	}

	QTextEdit::keyPressEvent(e);
	kdebugf2();
}

void CustomInput::keyReleaseEvent(QKeyEvent *e)
{
	bool handled = false;
	emit keyReleased(e, this, handled);
	if (handled)
	{
		e->accept();
		return;
	}
	if (e->key() == Qt::Key_Tab)
	{
		insertPlainText("    ");
		e->accept();
		return;
	}

	QTextEdit::keyReleaseEvent(e);
}

void CustomInput::contextMenuEvent(QContextMenuEvent *e)
{
	QScopedPointer<QMenu> menu(m_customInputMenuManager->menu(this));

	QAction *undo = new QAction(tr("Undo"), menu.data());
	undo->setShortcut(QKeySequence::Undo);
	connect(undo, SIGNAL(triggered()), this, SLOT(undo()));
	menu->addAction(undo);

	QAction *redo = new QAction(tr("Redo"), menu.data());
	redo->setShortcut(QKeySequence::Redo);
	connect(redo, SIGNAL(triggered()), this, SLOT(redo()));
	menu->addAction(redo);

	menu->addSeparator();

	QAction *cut = new QAction(tr("Cut"), menu.data());
	cut->setShortcut(QKeySequence::Cut);
	connect(cut, SIGNAL(triggered()), this, SLOT(cut()));
	menu->addAction(cut);

	QAction *copy = new QAction(tr("Copy"), menu.data());
	copy->setShortcut(QKeySequence::Copy);
	connect(copy, SIGNAL(triggered()), this, SLOT(copy()));
	menu->addAction(copy);

	QAction *paste = new QAction(tr("Paste"), menu.data());
	paste->setShortcut(QKeySequence::Paste);
	connect(paste, SIGNAL(triggered()), this, SLOT(paste()));
	menu->addAction(paste);

	auto pasteAndSend = new QAction(tr("Paste and send"), menu.data());
	connect(pasteAndSend, SIGNAL(triggered()), this, SLOT(pasteAndSend()));
	menu->addAction(pasteAndSend);

	QAction *clear = new QAction(tr("Clear"), menu.data());
	connect(clear, SIGNAL(triggered()), this, SLOT(clear()));
	menu->addAction(clear);

	menu->addSeparator();

	QAction *all = new QAction(tr("Select All"), menu.data());
	all->setShortcut(QKeySequence::SelectAll);
	connect(all, SIGNAL(triggered()), this, SLOT(selectAll()));
	menu->addAction(all);

	menu->exec(e->globalPos());
}

void CustomInput::setAutoSend(bool on)
{
	autosend_enabled = on;
}

void CustomInput::cursorPositionChangedSlot()
{
	emit fontChanged(currentFont());
}

void CustomInput::setCopyPossible(bool available)
{
	CopyPossible = available;
}

void CustomInput::pasteAndSend()
{
	paste();
	emit sendMessage();
}

bool CustomInput::canInsertFromMimeData(const QMimeData *source) const
{
	if (CurrentChat.chatAccount().protocolHandler() && CurrentChat.chatAccount().protocolHandler()->chatImageService())
	{
		if (source->hasUrls())
			return true;
		if (source->hasFormat(QStringLiteral("application/x-qt-image")))
			return true;
	}
	return QTextEdit::canInsertFromMimeData(source);
}

void CustomInput::acceptPlainText(QString plainText)
{
	insertPlainText(plainText.replace("\t", "    "));
}

void CustomInput::acceptFileUrl(QUrl imageUrl)
{
	if (!CurrentChat.chatAccount().protocolHandler() || !CurrentChat.chatAccount().protocolHandler()->chatImageService())
		return;

	imageUrl = m_imageStorageService->toFileUrl(imageUrl);
	if (!imageUrl.toString().isEmpty() && imageUrl.scheme() == "file")
	{
		auto path = QDir::cleanPath(imageUrl.path());
		if (QImage(path).isNull())
			return;
		insertHtml(QString{"<img src='%1' />"}.arg(path));
	}
}

void CustomInput::acceptImageData(QByteArray imageData)
{
	if (!CurrentChat.chatAccount().protocolHandler() || !CurrentChat.chatAccount().protocolHandler()->chatImageService())
		return;

	QBuffer buffer{&imageData};
	buffer.open(QIODevice::ReadOnly);
	auto ext = QString::fromUtf8(QImageReader{&buffer}.format().toLower());
	auto filename = QString{"drop%1.%2"}.arg(QDateTime::currentDateTime().toTime_t()).arg(ext);

	auto path = m_imageStorageService->fullPath(filename);
	QFile file(path);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		file.write(imageData);
		file.close();

		insertHtml(QString{"<img src='%1' />"}.arg(path));
	}
	else
		return;
}

void CustomInput::insertFromMimeData(const QMimeData *source)
{
	acceptPasteData(source, this);
}

#include "moc_custom-input.cpp"
