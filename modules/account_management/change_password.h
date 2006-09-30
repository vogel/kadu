#ifndef CHANGE_PASSWORD_H
#define CHANGE_PASSWORD_H

#include <qhbox.h>
#include <qlineedit.h>
#include <qevent.h>

class LayoutHelper;
class QResizeEvent;
/** @ingroup account_management
 * @{
 */
class ChangePassword : public QHBox {
	Q_OBJECT
	public:
		ChangePassword(QDialog *parent = 0, const char *name = 0);
		~ChangePassword();

	private:
		QLineEdit *emailedit, *newpwd, *newpwd2;
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
