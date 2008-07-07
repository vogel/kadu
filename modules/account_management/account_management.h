#ifndef ACC_MAN_H
#define ACC_MAN_H

#include <QtCore/QObject>

class ActionDescription;
class QAction;
/**
 * @defgroup account_management "Account management"
 * @{
 */
class AccountManagement : public QObject
{
	Q_OBJECT
	
		ActionDescription *remindMenuActionDescription;
		ActionDescription *changeMenuActionDescription;
		ActionDescription *registerMenuActionDescription;
		ActionDescription *unregisterMenuActionDescription;
	
	public:
		AccountManagement();
		~AccountManagement();
	
	public slots:
		void registerUser(QAction *sender, bool toggled);
		void unregisterUser(QAction *sender, bool toggled);
		void remindPassword(QAction *sender, bool toggled);
		void changePassword(QAction *sender, bool toggled);
};

extern AccountManagement *account_management;

/** @} */

#endif
