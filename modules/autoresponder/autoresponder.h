#ifndef AUTORESPONDER_H
#define AUTORESPONDER_H

#include <QtCore/QObject>
#include <QtCore/QSet>

#include "accounts/accounts-aware-object.h"
#include "configuration/configuration-aware-object.h"
#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "gui/windows/main-configuration-window.h"
#include "protocols/protocol.h"

/**
 * @defgroup autoresponder Autoresponder
 * @{
 */

class QLineEdit;
class ChatWidget;

class AutoResponder : public ConfigurationUiHandler, ConfigurationAwareObject, AccountsAwareObject
{
	Q_OBJECT

	private:
		ContactSet repliedUsers; /*!< kontakty, którym już odpowiedziano */

		QString autoRespondText; /*!< treść automatycznej odpowiedzi */

		QLineEdit *autoRespondTextLineEdit;

		bool respondConferences; /*!< czy odpowiadać na konkerencje */
		bool respondOnlyFirst; /*!< czy odpowiadać tylko na pierwszą wiadomość */

		bool statusAvailable; /*!< czy odpowiadamy, gdy jesteśmy dostępni */
		bool statusBusy; /*!< czy odpowiadamy, gdy jesteśmy zajęci */
		bool statusInvisible; /*!< czy odpowiadamy, gdy jesteśmy niewidoczni */

		void createDefaultConfiguration();

	protected:
		virtual void accountRegistered(Account account);
		virtual void accountUnregistered(Account account);

		/**
			\fn void configurationUpdated()
			Metoda jest wywoływana po zmianie w oknie konfiguracyjnym.
		**/
		virtual void configurationUpdated();

	public:
		/**
			\fn AutoResponder(QObject *parent=0)
			Standardowy konstruktor
			\param parent rodzic - domyślnie 0
		**/
		AutoResponder(QObject *parent=0);
		virtual ~AutoResponder();
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	public slots:
		void filterIncomingMessage(Chat chat, Contact sender, QString &message, time_t time, bool &ignore);

		/**
			\fn void chatOpenedClosed(ChatWidget *chat)
			Slot jest wywoływany przy otwieraniu lub po zamknięciu
			okna chat.
			\param chat okno wiadomości
		**/
		void chatOpenedClosed(ChatWidget *chat, bool activate = false);
};

extern AutoResponder* autoresponder;

/** @} */

#endif
