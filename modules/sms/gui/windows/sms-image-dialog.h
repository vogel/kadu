#ifndef SMS_IMAGE_DIALOG_H
#define SMS_IMAGE_DIALOG_H

#include <QtCore/QBuffer>
#include <QtCore/QMap>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

class SmsImageDialog : public QDialog
{
	Q_OBJECT
		QLineEdit* code_edit;

	private slots:
		void onReturnPressed();

	public:
		SmsImageDialog(QWidget* parent, const QByteArray& image);
		void reject();

	signals:
		void codeEntered(const QString& code);
};

#endif //SMS_IMAGE_DIALOG_H
