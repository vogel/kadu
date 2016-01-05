/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QRegExp>
#include <injeqt/injeqt.h>

class ChatWidget;
class ChatWidgetRepository;
class UserGroup;

/*!
 * This class is responsible for fixing small mistakes (and not only)
 * while sending chat messages, like "<i>nei</i>" to "<i>nie</i>", etc.
 * Generally it replaces one word with another, assigned to first one.
 * \class WordFix
 * \brief Words fixing.
 */
class WordFix : public QObject
{
	Q_OBJECT

	QPointer<ChatWidgetRepository> chatWidgetRepository;

	/*!
	 * \var QMap<QString,QString> wordsList
	 * Contains whole list of words for replacing in form:
	 * wordList["to_replace"] = "correct_word";
	 */
	QMap<QString,QString> m_wordsList;

	QRegExp ExtractBody;

private slots:
	INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);

	/*!
	 * \fn void chatCreated(ChatWidget *chat)
	 * This slot calls connectToClass"("Chat* chat")" to connect
	 * the object to chat "<i>send message</i>" signal.
	 * \param chat Pointer to the created chat window.
	 */
	void chatWidgetAdded(ChatWidget *chat);

	/*!
	 * \fn void chatDestroying(ChatWidget *chat)
	 * This slot calls disconnectFromClass"("Chat* chat")" to disconnect
	 * the object from chat "<i>send message</i>" signal.
	 * \param chat Pointer to the chat window.
	 */
	void chatWidgetRemoved(ChatWidget *chat);

	/*!
	 * \fn void sendRequest(Chat* chat)
	 * Handles chat message send request.
	 * It checks configuration if words fixing is enabled. If it's not,
	 * then the method makes no changes in input text.
	 * \param chat Chat window which sending request is invoked by.
	 */
	void sendRequest(ChatWidget *chat);

public:
	/*!
	 * \fn WordFix()
	 * Default constructor. Reads words list or (if it's not defined yet) loads default list.
	 */
	Q_INVOKABLE explicit WordFix(QObject *parent = nullptr);

	/*!
	 * \fn ~WordFix()
	 * Default destructor.
	 */
	virtual ~WordFix();

	QMap<QString,QString> & wordsList() { return m_wordsList; }

};
