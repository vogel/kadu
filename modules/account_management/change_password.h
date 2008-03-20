#ifndef CHANGE_PASSWORD_H
#define CHANGE_PASSWORD_H

#include <QWidget>

class LayoutHelper;
class QLineEdit;
class QResizeEvent;
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
		LayoutHelper *layoutHelper;

	private slots:
		void start();
		void passwordChanged(bool ok);
		void keyPressEvent(QKeyEvent *);

	protected:
		virtual void resizeEvent(QResizeEvent *);
};

/** @} */

#endif
