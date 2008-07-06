#ifndef HISTORY_SEARCH_DIALOG_H
#define HISTORY_SEARCH_DIALOG_H

#include <QtGui/QDialog>

#include "gadu.h"

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLineEdit;
class QRadioButton;

struct HistoryFindRec {
	QDateTime fromdate;
	QDateTime todate;
	int type;
	QString data;
	bool reverse;
	int actualrecord;
	HistoryFindRec();
};

class HistorySearchDialog : public QDialog 
{
	Q_OBJECT

	protected:
		QGroupBox *fromGroupBox, *toGroupBox, *phraseGroupBox, *statusGroupBox;
		QCheckBox *fromCheckBox, *toCheckBox, *reverseCheckBox;
		QComboBox *fromDayComboBox, *fromMonthComboBox, *fromYearComboBox, *fromHourComboBox, *fromMinComboBox;
		QComboBox *toDayComboBox, *toMonthComboBox, *toYearComboBox, *toHourComboBox, *toMinComboBox;
		QComboBox *statusComboBox;
		QLineEdit *phraseEdit;
		QButtonGroup *criteriaButtonGroup;
		QGroupBox *criteriaGroupBox;
		QRadioButton *phraseRadioButton , *statusRadioButton ;
		QStringList numsList;
		UinsList uins;

		void resetFromDate();
		void resetToDate();
	public:
		HistorySearchDialog(QWidget *parent, UinsList uins);
		void setDialogValues(HistoryFindRec &findrec);
		HistoryFindRec getDialogValues() const;

	public slots:
		void correctFromDays(int index);
		void correctToDays(int index);
		void fromToggled(bool on);
		void toToggled(bool on);
		void criteriaChanged(int id);
		void findBtnClicked();
		void cancelBtnClicked();
		void resetBtnClicked();
};

#endif
