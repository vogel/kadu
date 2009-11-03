#ifndef SMS_GATEWAY_MANAGER_H
#define SMS_GATEWAY_MANAGER_H

#include <QtCore/QMap>

#include "sms.h"

class MobileNumber : public QObject
{
	Q_OBJECT
	QString Number;
	QString GatewayId;
	
  public:
	MobileNumber() {};
	virtual ~MobileNumber() {};
};

class SMSAPI MobileNumberManager : public StorableObject
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
	
	void registerNumber(QString &number, QString &gatewayId);
	void unregisterNumber(QString &number);

	QMap<MobileNumber *, QString> numbers() { return Numbers; };
	
};

#endif // SMS_GATEWAY_MANAGER_H
