#ifndef HISTORY_SEARCH_DIALOG_H
#define HISTORY_SEARCH_DIALOG_H

#include <qdialog.h>

#include "gadu.h"

class QCheckBox;
class QComboBox;
class QHGroupBox;
class QLineEdit;
class QRadioButton;
class QVButtonGroup;

struct HistoryFindRec {
	QDateTime fromdate;
	QDateTime todate;
	int type;
	QString data;
	bool reverse;
	int actualrecord;
	HistoryFindRec();
};

class HistorySearchDialog : public QDialog {
	Q_OBJECT
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

	protected:
		QHGroupBox *from_hgb, *to_hgb, *phrase_hgb, *status_hgb;
		QCheckBox *from_chb, *to_chb, *reverse_chb;
		QComboBox *from_day_cob, *from_month_cob, *from_year_cob, *from_hour_cob, *from_min_cob;
		QComboBox *to_day_cob, *to_month_cob, *to_year_cob, *to_hour_cob, *to_min_cob;
		QComboBox *status_cob;
		QLineEdit *phrase_edit;
		QVButtonGroup *criteria_bg;
		QRadioButton *phrase_rb, *status_rb;
		QStringList numslist;
		UinsList uins;

		void resetFromDate();
		void resetToDate();
};

#endif
