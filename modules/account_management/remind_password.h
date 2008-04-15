#ifndef REMIND_PASSWORD_H
#define REMIND_PASSWORD_H

#include <QWidget>

class QLineEdit;

/**
 * @ingroup account_management
 * @}
 */
class RemindPassword : public QWidget
{
	Q_OBJECT

	private:
		QLineEdit* emailedit;

	private slots:
		void start();
		void reminded(bool ok);
		void keyPressEvent(QKeyEvent *);

	public:
		RemindPassword(QDialog *parent = 0, const char *name = "remind_pasword");
		~RemindPassword();

};

/** @} */

#endif
