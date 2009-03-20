#ifndef KADU_CHOOSE_DESCRIPTION_H
#define KADU_CHOOSE_DESCRIPTION_H

#include "base.h"

class ChooseDescription : public QDialog
{
	Q_OBJECT

	static ChooseDescription *Dialog;

	QPushButton *OkButton;
	QComboBox *Description;
	QLabel *AvailableChars;

	Status CurrentStatus;

private slots:
	void okPressed();
	void cancelPressed();
	void updateAvailableChars(const QString &);

public:
	static void show(const Status &status, const QPoint &position);

	ChooseDescription(QWidget *parent = 0);
	virtual ~ChooseDescription();

	void setStatus(const Status &status);
	void setPosition(const QPoint &position);
};

#endif
