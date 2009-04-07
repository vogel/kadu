#ifndef AUTORESPONDER_H
#define AUTORESPONDER_H

#include <QtCore/QObject>
#include <QtCore/QList>

#include "config_file.h"
#include "configuration_aware_object.h"
#include "main_configuration_window.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
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
		QList<UserListElements> repliedUsers; /*!< uiny os�b, kt�rym ju� odpowiedziano */

		QString autotext; /*!< tre�� automatycznej odpowiedzi */

		bool respondConferences; /*!< czy odpowiada� na konkerencje */
		bool respondOnlyFirst; /*!< czy odpowiada� tylko na pierwsz� wiadomo�� */

		bool statusAvailable; /*!< czy odpowiadamy, gdy jeste�my dost�pni */
		bool statusBusy; /*!< czy odpowiadamy, gdy jeste�my zaj�ci */
		bool statusInvisible; /*!< czy odpowiadamy, gdy jeste�my niewidoczni */

		void createDefaultConfiguration();

	protected:
		/**
			\fn void configurationUpdated()
			Metoda jest wywo�ywana po zmianie w oknie konfiguracyjnym.
		**/
		virtual void configurationUpdated();

	public:
		/**
			\fn AutoResponder(QObject *parent=0, const char *name=0)
			Standardowy konstruktor
			\param parent rodzic - domy�lnie 0
			\param name nazwa - domy�lnie 0
		**/
		AutoResponder(QObject *parent=0, const char *name=0);
		virtual ~AutoResponder();
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	public slots:

		/**
			\fn void messageReceived(Protocol *protocol, UserListElements senders, const QString& msg, time_t time)
			Slot jest wywo�ywany po odebraniu wiadomo�ci.
			\param protocol protok�� po kt�rym nadesz�a wiadomo��
			\param senders lista nadawc�w wiadomo�ci
			\param msg tre�� wiadomo�ci
			\param time czas
		**/
		void messageReceived(Protocol *protocol, UserListElements senders, const QString& msg, time_t time);

		/**
			\fn void chatOpenedClosed(ChatWidget *chat)
			Slot jest wywo�ywany przy otwieraniu lub po zamkni�ciu
			okna chat.
			\param chat okno wiadomo�ci
		**/
		void chatOpenedClosed(ChatWidget *chat);
};

extern AutoResponder* autoresponder;

/** @} */

#endif
