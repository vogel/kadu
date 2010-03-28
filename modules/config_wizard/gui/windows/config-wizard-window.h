#ifndef KADU_CONFIG_WIZARD_H
#define KADU_CONFIG_WIZARD_H

#include <QtCore/QList>
#include <QtGui/QGridLayout>
#include <QtGui/QWizard>

#include "misc/misc.h"

class ActionDescription;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QRadioButton;

class ConfigWizardPage;

/**
 * @defgroup config_wizard Config wizard
 * @{
 */
class ConfigWizardWindow : public QWizard
{
	Q_OBJECT

	QList<ConfigWizardPage *> ConfigWizardPages;

	QComboBox *browserCombo;
	QLineEdit *browserCommandLineEdit;
	QComboBox *mailCombo;
	QLineEdit *mailCommandLineEdit;

	QComboBox *soundModuleCombo;
	QPushButton *soundTest;

	QString backupSoundModule;

	void createApplicationsPage();
	void createSoundPage();

	void loadApplicationsOptions();
	void loadSoundOptions();

	void saveApplicationsOptions();
	void saveSoundOptions();

private slots:
	void acceptedSlot();
	void rejectedSlot();

public:
	explicit ConfigWizardWindow(QWidget *parent = 0);
	virtual ~ConfigWizardWindow();

	void addPage(ConfigWizardPage *page);

public slots:
	void wizardStart();

};

/** @} */

#endif
