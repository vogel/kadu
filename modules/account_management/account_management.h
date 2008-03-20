#ifndef ACC_MAN_H
#define ACC_MAN_H

#include <QObject>

class QAction;
/**
 * @defgroup account_management "Account management"
 * @{
 */
class AccountManagement : public QObject
{
	Q_OBJECT

	private:
		QAction *remindMenuAction;
		QAction *changeMenuAction;
		QAction *registerMenuAction;
		QAction *unregisterMenuAction;
	
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
