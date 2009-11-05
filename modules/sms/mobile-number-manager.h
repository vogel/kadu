#ifndef MOBILE_NUMBER_MANAGER_H
#define MOBILE_NUMBER_MANAGER_H

#include <QtCore/QMap>

#include "configuration/storable-object.h"

class SmsGateway;

class MobileNumber : public StorableObject
{
	QString Number;
	QString GatewayId;
	
public:
	MobileNumber() {};
	MobileNumber(QString number, QString gatewayId);
	virtual ~MobileNumber() {};
	
	virtual void load();
	virtual void store();
	
	QString number() { return Number; };
	void setNumber(QString number) { Number = number; };
	
	QString gatewayId() { return GatewayId; };
	void setGatewayId(QString gatewayId) { GatewayId = gatewayId; };
	
protected:
	virtual StoragePoint * createStoragePoint();
};

class MobileNumberManager : public StorableObject
{
	Q_DISABLE_COPY(MobileNumberManager)

	static MobileNumberManager *Instance;
	QMap<MobileNumber *, QString> Numbers;
	
	MobileNumberManager(QObject* parent = 0) {};
	virtual ~MobileNumberManager() {};
	
public:
	static MobileNumberManager * instance();  
	
	virtual void load();
	virtual void store();
	
	void registerNumber(QString number, QString gatewayId);
	void unregisterNumber(QString number);
	SmsGateway * gateway(QString number);
	

	QMap<MobileNumber *, QString> numbers() { return Numbers; };
	
protected:
	virtual StoragePoint * createStoragePoint();
	
};

#endif // MOBILE_NUMBER_MANAGER_H
