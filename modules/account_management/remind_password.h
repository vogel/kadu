#ifndef REMIND_PASSWORD_H
#define REMIND_PASSWORD_H

#include <QtGui/QWidget>

class QLineEdit;

/**
 * @ingroup account_management
 * @}
 */
class RemindPassword : public QWidget
{
	Q_OBJECT

		QLineEdit* emailedit;

	private slots:
		void start();
		void reminded(bool ok);
		void keyPressEvent(QKeyEvent *);

	public:
		RemindPassword(QDialog *parent = 0);
		~RemindPassword();

};

/** @} */

#endif
