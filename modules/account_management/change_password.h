#ifndef CHANGE_PASSWORD_H
#define CHANGE_PASSWORD_H

#include <QWidget>

class QLineEdit;
/** @ingroup account_management
 * @{
 */
class ChangePassword : public QWidget 
{
	Q_OBJECT

		QLineEdit *emailedit;
		QLineEdit *newpwd;
		QLineEdit *newpwd2;

	private slots:
		void start();
		void passwordChanged(bool ok);
		void keyPressEvent(QKeyEvent *);

	public:
		ChangePassword(QDialog *parent = 0);
		~ChangePassword();
};

/** @} */

#endif
