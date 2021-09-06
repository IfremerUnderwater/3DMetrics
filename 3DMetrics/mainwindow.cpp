
#include "Mainwindow.h"


Mainwindow::Mainwindow()
{

    QMenu *menuFichier = menuBar()->addMenu("&Fichier");


    QAction *actionQuitter = new QAction("&Quitter", this);

    menuFichier->addAction(actionQuitter);


    QMenu *menuEdition = menuBar()->addMenu("&Edition");

    QMenu *menuAffichage = menuBar()->addMenu("&Affichage");




    QWidget *zoneCentrale = new QWidget;


    QLineEdit *nom = new QLineEdit;

    QLineEdit *prenom = new QLineEdit;

    QLineEdit *age = new QLineEdit;


    QFormLayout *layout = new QFormLayout;

    layout->addRow("Votre nom", nom);

    layout->addRow("Votre prénom", prenom);

    layout->addRow("Votre âge", age);


    zoneCentrale->setLayout(layout);

    setCentralWidget(zoneCentrale);
}

