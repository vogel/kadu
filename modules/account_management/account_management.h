#ifndef ACC_MAN_H
#define ACC_MAN_H

#include <qobject.h>
/**
 * @defgroup account_management "Account management"
 * @{
 */
class AccountManagement : public QObject
{
	Q_OBJECT

	private:
		int remindMenuId;
		int changeMenuId;
		int registerMenuId;
		int unregisterMenuId;
	
	public:
		AccountManagement();
		~AccountManagement();
	
	public slots:
		void registerUser();
		void unregisterUser();
		void remindPassword();
		void changePassword();
};

extern AccountManagement *account_management;

/** @} */

#endif
