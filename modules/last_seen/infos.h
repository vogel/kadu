#ifndef INFOS_H
#define INFOS_H

#include <QObject>
#include <QMap>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "status/status.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"

//! A "dictionary oriented" list type, holding uins and "Last seen" times.
typedef QMap<QPair<QString, QString>, QString> LastSeen;

/*!
 * This class handles the "Last seen" time for InfosDialog class.
 * \brief "Last seen, IP, DNS" main class.
 */
class Infos : public QObject
{
	Q_OBJECT
	public:
		/*!
		 * \brief Default contructor.
		 * \param parent - the parent object,
		 * \param name - the object's name
		 */
		Infos(QObject *parent = 0);

		//! Default destructor
		~Infos();
		
	public slots:
		//! This slot open InfosDialog dialog.
		void onShowInfos();

	private slots:
		void accountRegistered(Account);
		void accountUnregistered(Account);

		void contactStatusChanged(Contact contact, Status status);

	protected:
		//! updates Last Seen times in lastSeen.
		void updateTimes();

		/*!
		 * It holds the data file name. It's set to <i>profilePath("last_seen.data")</i>
		 * in the constructor.
		 */
		QString fileName;
		
		/*!
		 * List with "last seen on" time.
		 * The keys are uins,
		 * the values are "last seen" times.
		 */
		LastSeen lastSeen;
		
		//! Holds the "&Show infos about contacts" menu entry ID.
		int menuID;
		
		ActionDescription* lastSeenActionDescription;
};

#endif
