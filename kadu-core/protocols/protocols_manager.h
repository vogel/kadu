#ifndef KADU_PROTOCOLS_MANAGER_H
#define KADU_PROTOCOLS_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "exports.h"

class Protocol;
class ProtocolFactory;

class KADUAPI ProtocolsManager : public QObject
{
	static ProtocolsManager * Instance;

	ProtocolsManager();
	~ProtocolsManager();

	QMap<QString, ProtocolFactory *> registeredFactories;

public:
	static ProtocolsManager * instance();

	void registerProtocolFactory(const QString &name, ProtocolFactory *factory);
	void unregisterProtocolFactory(const QString &name);

	const QList<ProtocolFactory *> protocolFactories() { return registeredFactories.values(); }
	ProtocolFactory * protocolFactory(const QString &name);

	Protocol * newInstance(const QString &name);

};

#endif
