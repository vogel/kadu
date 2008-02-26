#ifndef KADU_PROTOCOLS_MANAGER_H
#define KADU_PROTOCOLS_MANAGER_H

#define QT3_SUPPORT
#include <qglobal.h>

#include <qobject.h>
#include <qstring.h>
#include <q3valuelist.h>

#include "protocol.h"

class ProtocolManager : public QObject
{
	Q_OBJECT
	public slots:
		virtual Protocol *newInstance(const QString &id) = 0;
};


class ProtocolsManager : public QObject
{
	private:
		ProtocolsManager();
		~ProtocolsManager();
		struct ProtocolDescription
		{
			QString protocolID;
			QString Name;
			ProtocolManager *Manager;
			
			ProtocolDescription(const QString &id, const QString &name, ProtocolManager *manager)
				: protocolID(id), Name(name), Manager(manager) {}
			ProtocolDescription() : protocolID(), Name(), Manager(0) {}
			ProtocolDescription(const ProtocolDescription &c) 
				: protocolID(c.protocolID), Name(c.Name), Manager(c.Manager) {}
		};
		Q3ValueList<ProtocolDescription> protocolDescriptions;
		Q3ValueList<Protocol *> protocols;

	public:
		static void initModule();
		static void closeModule();

		Q3ValueList<Protocol *> byProtocolID(const QString &protocolID);
		Protocol *byID(const QString &protocolID, const QString &ID);
		
		void registerProtocol(const QString &protocolID, const QString &name, ProtocolManager *manager);
		void unregisterProtocol(const QString &protocolID);
		Protocol *newProtocol(const QString &protocolID, const QString &ID);
};

extern ProtocolsManager *protocols_manager;

#endif
