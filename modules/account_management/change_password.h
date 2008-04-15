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

	public:
		ChangePassword(QDialog *parent = 0, const char *name = "change_password");
		~ChangePassword();

	private:
		QLineEdit *emailedit;
		QLineEdit *newpwd;
		QLineEdit *newpwd2;

	private slots:
		void start();
		void passwordChanged(bool ok);
		void keyPressEvent(QKeyEvent *);

};

/** @} */

#endif
