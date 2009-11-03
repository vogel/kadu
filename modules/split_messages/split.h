#ifndef SPLIT_H
#define SPLIT_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QTimer>

#include "configuration_aware_object.h"
#include "buddies/buddy-list.h"
#include "main_configuration_window.h"

class ChatWidget;
class BuddyList;

void fillEditor(ChatWidget *chat, QStringList &messages);

/*!
 * This class handles splitting messages longer than 1999 characters
 * \brief This class handles splitting messages longer than 1999 characters
 */
class Split : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT
	public:
		/*!
		 * \brief Default constructor
		 * \param parent points to patent's object.
		 * \param name is an object's name.
		 */
		Split(QObject *parent = 0, const char *name = 0);

		//! Default destructor.
		~Split();

		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	private slots:
		/*!
		 * \brief Called when new chat window is created
		 * \param chat a pointer to the created chat window
		 */
		void onChatCreated(ChatWidget *chat, time_t time);
		
		/*!
		 * \brief Called when user sends the message (e. g. by pressing "Send" button)
		 * \param chat a pointer to chat window
		 */
		void onMessageSendRequested(ChatWidget *chat);
		//void enable(bool b);
	
	private:
		//! Holds the list of messages to send
		QStringList messages;
		void enable(bool b);

	protected:
		virtual void configurationUpdated();
};

/*!
 * This class is used to send long message in parts
 * \brief This class is used to send long message in parts
 * \author Patryk Cisek (Prezu, patryk@prezu.one.pl)
 */
class SendSplitted : public QObject
{
	Q_OBJECT
	public:
		/*!
		 * \brief Default constructor
		 * \param chat a chat window to send the messages
		 * \param messagesToSend parts of long message to send
		 * \param parent points to patent's object.
		 * \param name is an object's name.
		 */
		SendSplitted(ChatWidget *chat, QStringList messagesToSend, QObject *parent = 0, const char *name = 0);
		
		//! Default destructor.
		~SendSplitted();
		
	private slots:
		//! Deletes the the object
		void onDestroyThis();
		
		/*!
		 * \brief Called when part of a message's been sent and delivered
		 * \param receivers list of message's receivers (unused here)
		 * \param message the message (not used here)
		 */
		void onMessageSent(BuddyList receivers, const QString &message);
		
		//! Called when next part of the message is to be sent
		void sendNextPart();
		
	private:
		//! List of splited long message
		QStringList messages;
		
		QTimer timer;

		/*!
		 * This timer destroys the SendSplitted object after 2 minutes (avoids mem leak when user
		 * aborts sending the message
		 */
		QTimer destroingTimer;
		
		//! pointer to current SendSplitted instance's chat window
		ChatWidget *chatWindow;
};

extern Split *split;

#endif
