#ifndef REMIND_PASSWORD_H
#define REMIND_PASSWORD_H

#include <qhbox.h>
#include <qlineedit.h>
#include <qevent.h>

class LayoutHelper;
class QResizeEvent;
/**
 * @ingroup account_management
 * @}
 */
class RemindPassword : public QHBox
{
	Q_OBJECT

	private:
		QLineEdit* emailedit;
		LayoutHelper *layoutHelper;

	private slots:
		void start();
		void reminded(bool ok);
		void keyPressEvent(QKeyEvent *);

	public:
		RemindPassword(QDialog *parent = 0, const char *name = "remind_pasword");
		~RemindPassword();

	protected:
		virtual void resizeEvent(QResizeEvent *);
};

/** @} */

#endif
