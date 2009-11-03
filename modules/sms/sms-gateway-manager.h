#ifndef SMS_GATEWAY_MANAGER_H
#define SMS_GATEWAY_MANAGER_H

#include <QtCore/QMap>

#include "sms.h"
#include "sms-gateway.h"

class SMSAPI SmsGatewayManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(SmsGatewayManager)

	static SmsGatewayManager *Instance;
	QMap<QString, SmsGateway*> Gateways;
	
	SmsGatewayManager(QObject* parent = 0) {};
	virtual ~SmsGatewayManager() {};
	
public:
	static SmsGatewayManager * instance();    
	
	void registerGateway(SmsGateway *gateway);
	void unregisterGateway(QString);

	QMap<QString, SmsGateway *> gateways() { return Gateways; };
	
};

#endif // SMS_GATEWAY_MANAGER_H
