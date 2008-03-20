#ifndef REMIND_PASSWORD_H
#define REMIND_PASSWORD_H

#include <QWidget>

class LayoutHelper;
class QLineEdit;
class QResizeEvent;
/**
 * @ingroup account_management
 * @}
 */
class RemindPassword : public QWidget
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
