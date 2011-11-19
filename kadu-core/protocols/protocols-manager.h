#ifndef KADU_PROTOCOLS_MANAGER_H
#define KADU_PROTOCOLS_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "exports.h"

class Protocol;
class ProtocolFactory;

class KADUAPI ProtocolsManager : public QObject
{
	Q_OBJECT

	static ProtocolsManager * Instance;

	ProtocolsManager();
	virtual ~ProtocolsManager();

	void init();

	QList<ProtocolFactory *> Factories;

public:
	static ProtocolsManager * instance();

	void registerProtocolFactory(ProtocolFactory *Factory);
	void unregisterProtocolFactory(ProtocolFactory *Factory);

	const QList<ProtocolFactory *> & protocolFactories() const { return Factories; }
	bool hasProtocolFactory(const QString &name);

	int count() { return Factories.count(); }

	ProtocolFactory * byName(const QString &name);
	ProtocolFactory * byIndex(int index);

	int indexOf(ProtocolFactory *protocolFactory) { return Factories.indexOf(protocolFactory); }

signals:
	void protocolFactoryAboutToBeRegistered(ProtocolFactory *factory);
	void protocolFactoryRegistered(ProtocolFactory *factory);
	void protocolFactoryAboutToBeUnregistered(ProtocolFactory *factory);
	void protocolFactoryUnregistered(ProtocolFactory *factory);

};

#endif // KADU_PROTOCOLS_MANAGER_H
