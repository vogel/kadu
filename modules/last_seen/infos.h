#ifndef INFOS_H
#define INFOS_H

#include <QObject>
#include <QMap>

#include "userlistelement.h"
#include "status.h"
#include "action.h"

//! A "dictionary oriented" list type, holding uins and "Last seen" times.
typedef QMap<QString, QString> LastSeen;

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
		Infos(QObject *parent = 0, const char *name = 0);

		//! Default destructor
		~Infos();
		
	public slots:
		//! This slot open InfosDialog dialog.
		void onShowInfos();

	protected slots:
		/*!
		 * \brief Updates "last seen" time for elem.
		 * \param protocolName - the protol's name. Currently only "Gadu" is supported,
		 * \param elem - userlist's element, that the currentValue has changed for,
		 * \param name - the changed property's name,
		 * \param oldValue - the previous property's value,
		 * \param currentValue - the new property's value,
		 * \param massively - true when it's part of some major changes (not used here),
		 * \param last - true when massively is true and it's the last change (not used here).
		 */
		void onUserStatusChangedSlot(QString protocolName, UserListElement elem, QString name, QVariant oldValue,
				QVariant currentValue, bool massively, bool last);

	protected:
		//! updates Last Seen times in lastSeen.
		void updateTimes();

		/*!
		 * It holds the data file name. It's set to <i>ggPath("last_seen.data")</i>
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
