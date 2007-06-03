#ifndef SELECT_FILE_H
#define SELECT_FILE_H

#include <qhbox.h>

class QLineEdit;

class SelectFile : public QHBox
{
	Q_OBJECT

	QString Type;
	QLineEdit *LineEdit;

private slots:
	void selectFileClicked();

public:
	SelectFile(const QString &type, QWidget *parent = 0, char *name = 0);
	SelectFile(QWidget *parent = 0, char *name = 0);

	void setType(const QString &type) { Type = type; }

	QString file() const;
	void setFile(const QString &file);

};

#endif // SELECT_FILE_H
