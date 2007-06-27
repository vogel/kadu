#ifndef AUTORESPONDER_H
#define AUTORESPONDER_H

#include <qobject.h>

#include "config_file.h"
#include "configuration_aware_object.h"
#include "main_configuration_window.h"
#include "misc.h"
#include "protocol.h"
#include "usergroup.h"
/**
 * @defgroup autoresponder Autoresponder
 * @{
 */

class ChatWidget;

class AutoResponder : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	private:
		QValueList<UserListElements> repliedUsers; /*!< uiny osób, którym ju¿ odpowiedziano */

		QString autotext; /*!< tre¶æ automatycznej odpowiedzi */

		bool respondConferences; /*!< czy odpowiadaæ na konkerencje */
		bool respondOnlyFirst; /*!< czy odpowiadaæ tylko na pierwsz± wiadomo¶æ */

		bool statusAvailable; /*!< czy odpowiadamy, gdy jeste¶my dostêpni */
		bool statusBusy; /*!< czy odpowiadamy, gdy jeste¶my zajêci */
		bool statusInvisible; /*!< czy odpowiadamy, gdy jeste¶my niewidoczni */

		/**
			\fn void import_0_5_0_Configuration()
			Import danych konfiguracyjnych ze starszej wersji Kadu.
		**/
		void import_0_5_0_Configuration();

	protected:
		/**
			\fn void configurationUpdated()
			Metoda jest wywo³ywana po zmianie w oknie konfiguracyjnym.
		**/
		virtual void configurationUpdated();

	public:
		/**
			\fn AutoResponder(QObject *parent=0, const char *name=0)
			Standardowy konstruktor
			\param parent rodzic - domy¶lnie 0
			\param name nazwa - domy¶lnie 0
		**/
		AutoResponder(QObject *parent=0, const char *name=0);
		virtual ~AutoResponder();
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	public slots:

		/**
			\fn void messageReceived(Protocol *protocol, UserListElements senders, const QString& msg, time_t time)
			Slot jest wywo³ywany po odebraniu wiadomo¶ci.
			\param protocol protokó³ po którym nadesz³a wiadomo¶æ
			\param senders lista nadawców wiadomo¶ci
			\param msg tre¶æ wiadomo¶ci
			\param time czas
		**/
		void messageReceived(Protocol *protocol, UserListElements senders, const QString& msg, time_t time);

		/**
			\fn void chatOpenedClosed(ChatWidget *chat)
			Slot jest wywo³ywany przy otwieraniu lub po zamkniêciu
			okna chat.
			\param chat okno wiadomo¶ci
		**/
		void chatOpenedClosed(ChatWidget *chat);
};

extern AutoResponder* autoresponder;

/** @} */

#endif
