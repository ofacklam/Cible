#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(ui->lbl_path);

    m_leftPane = new LeftPane(ui->graphicsView, this);
    ui->mainContainer->insertWidget(0, m_leftPane);
    ui->mainContainer->setStretch(0, 1);
    ui->mainContainer->setStretch(1, 1);

    QAction *action = ui->menuBar->addAction("A propos de...");
    connect(action, SIGNAL(triggered(bool)), this, SLOT(openAboutDialog()));

    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(createFile()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(openFile()));
    connect(ui->btn_groups, SIGNAL(clicked(bool)), this, SLOT(openGroupsStudentsManager()));
    connect(ui->btn_competences, SIGNAL(clicked(bool)), this, SLOT(openCompetenceManager()));
    connect(ui->btn_reset, SIGNAL(clicked(bool)), m_leftPane, SLOT(resetCompetences()));
    connect(ui->btn_pdf, SIGNAL(clicked(bool)), m_leftPane, SLOT(printCompetences()));

    bool file_opened = false;
    // Ouvrir directement un fichier (double-clic)
    QStringList args = QCoreApplication::arguments();
    if(args.count() > 1) {
        QString suffix = QFileInfo(args[1]).suffix().toUpper();
        // Check the file suffix
        if(suffix == "CIBLE") {
            openFile(args[1]); // Try to open the file
            file_opened = true;
        } else {
            QMessageBox::critical(this, "Fichier non pris en charge", "Erreur : Fichier " + QFileInfo(args[1]).suffix().toUpper() + " non pris en charge.");
        }
    }

    //Try to load the default file and update window
    if(!file_opened) {
        Preferences pref;
        QString filename = pref.file();
        if(filename != "" && QFileInfo(filename).exists()) {
            openFile(filename);
        }
    }

    updateWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createFile() {
    //Try to load directory preferences
    Preferences pref;
    QString pref_path = pref.dir();

    //Get file name
    QString filename = QFileDialog::getSaveFileName(this, "Enregistrer sous...",
                                                    pref_path + QDir::separator() + "donnees",  "CIBLE (*.cible)");

    if(filename == "") {
        updateWindow();
        return;
    }

    //Save file in preferences
    pref.setFile(filename);

    if(kscopemanager.createFile(filename))
        QMessageBox::information(this, "Succès", "Votre fichier a été créé.<br />Vous pouvez dès maintenant l'utiliser.");

    updateWindow();
}

void MainWindow::openFile() {
    //Try to load directory preferences
    Preferences pref;
    QString pref_path = pref.dir();

    //Get file name
    QString fileDB = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", pref_path + QDir::separator(), "CIBLE (*.cible)");

    if(fileDB == "") {
        updateWindow();
        return;
    }

    QString suffix = QFileInfo(fileDB).suffix().toUpper();
    // Check the file suffix
    if(suffix == "CIBLE")
        openFile(fileDB); // Try to open the file
    else
        QMessageBox::critical(this, "Fichier non pris en charge", "Erreur : Fichier " + QFileInfo(fileDB).suffix().toUpper() + " non pris en charge.");

    return;
}

void MainWindow::openFile(QString filename) {
    //Save directory in preferences
    Preferences pref;
    pref.setFile(filename);

    kscopemanager.openFile(filename);
    updateWindow();
}

void MainWindow::openGroupsStudentsManager() {
    GroupsStudentsManager mng(this);
    mng.exec();
    updateWindow();
}

void MainWindow::openAboutDialog() {
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::openCompetenceManager() {
    Group *g = m_leftPane->selectedGroup();
    Student *s = m_leftPane->selectedStudent();

    CompetenceManager mng(g, s, this);
    mng.exec();
    m_leftPane->updatePagesBox();
}

//To complete !!!
void MainWindow::updateWindow() {
    //Check if DataBase is open
    QSqlDatabase db = QSqlDatabase::database();

    //Enable buttons and lists
    ui->btn_groups->setEnabled(db.isOpen());
    ui->btn_competences->setEnabled(db.isOpen());
    ui->btn_reset->setEnabled(db.isOpen());
    ui->btn_pdf->setEnabled(db.isOpen());
    m_leftPane->setEnabled(db.isOpen());

    //Update label
    ui->lbl_path->setText(db.isOpen() ? "Fichier chargé : " + db.databaseName() : "Aucun fichier chargé.");

    //Update screen
    if(db.isOpen()) {
        m_leftPane->updateGroupsList();
    }
    m_leftPane->updatePagesBox();
}
