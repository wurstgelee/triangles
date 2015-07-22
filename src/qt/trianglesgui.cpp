/*
 * Qt5 Triangles GUI.
 *
 * W.J. van der Laan 2011-2012
 * The Bitcoin developers 2011-2012
 * The Triangles Team 2014-2015 
 */
#include "trianglesgui.h"
#include "ui_mainwindow.h"
#include "transactiontablemodel.h"
#include "addressbookpage.h"
#include "messagepage.h"
#include "sendcoinsdialog.h"

#include "dialog_move_handler.h"
#include "optionsdialog.h"
#include "aboutdialog.h"

#include "signmessagepage.h"
#include "verifymessagepage.h"
#include "clientmodel.h"
#include "walletmodel.h"
#include "messagemodel.h"
#include "editaddressdialog.h"

#include "optionsmodel.h"
#include "transactiondescdialog.h"
#include "addresstablemodel.h"
#include "transactionview.h"
#include "overviewpage.h"
#include "trianglesunits.h"
#include "guiconstants.h"
#include "askpassphrasedialog.h"
#include "notificator.h"
#include "guiutil.h"
#include "rpcconsole.h"
#include "ui_interface.h"
#include "main.h"
#include "init.h"
#include "util.h"
//#include "message_box_dialog.h"
#include "wallet.h"

#ifdef Q_OS_MAC
#include "macdockiconhandler.h"
#endif

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QIcon>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLocale>
#include <QMessageBox>
#include <QProgressBar>
#include <QStackedWidget>
#include <QDateTime>
#include <QMovie>
#include <QFileDialog>
#include <QDesktopServices>
#include <QTimer>
#include <QDragEnterEvent>
#if QT_VERSION < 0x050000
#include <QUrl>
#endif
#include <QMimeData>
#include <QStyle>
#include <QStyleFactory>
#include <QTextStream>
#include <QTextDocument>
#include <QSettings>
#include <QDesktopWidget>
#include <QListWidget>
#include <QPainter>
//#include <QSound>
#include <QSizeGrip>

#include <iostream>

extern CWallet* pwalletMain;
extern int64_t nLastCoinStakeSearchInterval;
extern unsigned int nTargetSpacing;
double GetPoSKernelPS();
const QString TrianglesGUI::DEFAULT_WALLET = "~Default";

TrianglesGUI::TrianglesGUI(bool fIsTestnet, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    clientModel(0),
    walletModel(0),
    encryptWalletAction(0),
    changePassphraseAction(0),
    unlockWalletAction(0),
    unlockWalletStakingAction(0),
    lockWalletAction(0),
    aboutQtAction(0),
    trayIcon(0),
    notificator(0),
    rpcConsole(0),
    prevBlocks(0)
{

    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::Window);

    ui->wCaption->installEventFilter(new DialogMoveHandler(this));

    this->setStyleSheet(".TrianglesGUI \
                        QToolTip \
                        {\
                             color: #f26522;\
                             border: 1px solid #61280E;\
                             background-color: #000;\
                             /*padding: 10px 10px;*/\
                             border-radius: 3px;\
                             /*opacity: 200;*/\
                             min-height: 20px;\
                             /*min-width:60px;*/\
                        }\
                        QComboBox::drop-down {\
                        subcontrol-origin: padding;\
                        subcontrol-position: top right;\
                        /*width: 15px;*/\
                        border-left-width: 1px;\
                        border-left-color: #f26522;\
                        border-left-style: solid;\
                        }\
                        QComboBox::down-arrow {\
                        image: url(:/icons/down_arrow);\
                        }\
                        QMenu {\
                             background-color: #000; \
                             border: 1px solid #f26522;\
                             color: #f26522;\
                         }\
                         \
                         QMenu::item {\
                             background-color: transparent;\
                         }\
                         \
                         QMenu::item:selected {\
                             background-color: #61280E;\
                         }\
                        \
                        QScrollBar:horizontal {\
                             border: 1px solid #f26522;\
                             background: #000;\
                             height: 15px;\
                             margin: 0px 16px 0 16px;\
                        }\
                        \
                        QScrollBar::handle:horizontal\
                        {\
                              border: 1px solid #f26522;\
                              background: #1c1c1c;\
                              min-height: 20px;\
                              /*border-radius: 2px;*/\
                        }\
                        \
                        QScrollBar::add-line:horizontal {\
                              border: 1px solid #f26522;\
                              /*border-radius: 2px;*/\
                              background: #1c1c1c;\
                              width: 14px;\
                              subcontrol-position: right;\
                              subcontrol-origin: margin;\
                        }\
                        \
                        QScrollBar::sub-line:horizontal {\
                              border: 1px solid #f26522;\
                              /*border-radius: 2px;*/\
                              background: #1c1c1c;\
                              width: 14px;\
                             subcontrol-position: left;\
                             subcontrol-origin: margin;\
                        }\
                        \
                        QScrollBar::right-arrow:horizontal, QScrollBar::left-arrow:horizontal\
                        {\
                              border: 1px solid #f26522;\
                              width: 1px;\
                              height: 1px;\
                              background: #f26522;\
                        }\
                        \
                        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal\
                        {\
                              background: none;\
                        }\
                        \
                        QScrollBar:vertical\
                        {\
                              background: #000;\
                              width: 15px;\
                              margin: 16px 0 16px 0;\
                              border: 1px solid #f26522;\
                        }\
                        \
                        QScrollBar::handle:vertical\
                        {\
                              border: 1px solid #f26522;\
                              background: #1c1c1c;\
                              min-height: 20px;\
                              /*border-radius: 2px;*/\
                        }\
                        \
                        QScrollBar::add-line:vertical\
                        {\
                              border: 1px solid #f26522;\
                              /*border-radius: 2px;*/\
                              background: #1c1c1c;\
                              height: 14px;\
                              subcontrol-position: bottom;\
                              subcontrol-origin: margin;\
                        }\
                        \
                        QScrollBar::sub-line:vertical\
                        {\
                              border: 1px solid #f26522;\
                              /*border-radius: 2px;*/\
                              background: #1c1c1c;\
                              height: 14px;\
                              subcontrol-position: top;\
                              subcontrol-origin: margin;\
                        }\
                        \
                        QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical\
                        {\
                              border: 1px solid #f26522;\
                              width: 1px;\
                              height: 1px;\
                              background: #f26522;\
                        }\
                        \
                        \
                        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical\
                        {\
                              background: none;\
                        }\
                        ");

#ifndef Q_OS_MAC
    if (!fIsTestnet)
    {
        setWindowTitle(tr("Triangles") + " - " + tr("Wallet"));
        QApplication::setWindowIcon(QIcon(":icons/triangles"));
        setWindowIcon(QIcon(":icons/triangles"));
    }
    else
    {
        setWindowTitle(tr("Triangles") + " - " + tr("Wallet") + " " + tr("[testnet]"));
        QApplication::setWindowIcon(QIcon(":icons/triangles"));
        setWindowIcon(QIcon(":icons/triangles_testnet"));
    }
#else
    setUnifiedTitleAndToolBarOnMac(true);
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);

    if (!fIsTestnet)
        MacDockIconHandler::instance()->setIcon(QIcon(":icons/triangles"));
    else
        MacDockIconHandler::instance()->setIcon(QIcon(":icons/triangles_testnet"));
#endif
    // Accept D&D of URIs
    setAcceptDrops(true);

    // Create actions for the toolbar, menu bar and tray/dock icon
    // Needs walletFrame to be initialized
    createActions(fIsTestnet);


    // Create system tray icon and notification
    createTrayIcon();

    // Create tabs
    overviewPage = new OverviewPage();
    {
      transactionsPage = new QWidget(this);
      QHBoxLayout* hl = new QHBoxLayout(transactionsPage);
      QFrame* frameMain = new QFrame(transactionsPage);
      hl->addWidget(frameMain);
      frameMain->setFrameShape(QFrame::NoFrame);
      QVBoxLayout *vbox = new QVBoxLayout(frameMain);
      transactionView = new TransactionView(transactionsPage);
      vbox->addWidget(transactionView);
      frameMain->setLayout(vbox);
    }
    addressBookPage = new AddressBookPage(AddressBookPage::ForEditing, AddressBookPage::SendingTab);

    receiveCoinsPage = new AddressBookPage(AddressBookPage::ForEditing, AddressBookPage::ReceivingTab);

    sendCoinsPage = new SendCoinsDialog(this);
    messagePage   = new MessagePage(this);
	signMessagePage = new SignMessagePage(this);
    verifyMessagePage = new VerifyMessagePage(this);
    centralWidget = ui->stackedWidget;
    centralWidget->addWidget(overviewPage);
    centralWidget->addWidget(transactionsPage);
    centralWidget->addWidget(addressBookPage);
    centralWidget->addWidget(receiveCoinsPage);
    centralWidget->addWidget(sendCoinsPage);
    centralWidget->addWidget(messagePage);
	centralWidget->addWidget(signMessagePage);
    centralWidget->addWidget(verifyMessagePage);

    QSizeGrip* grip = new QSizeGrip(this);
    grip->setStyleSheet("width: 6px; height: 6px; image: url(:/res/icons/handle.png);");
    ui->horizontalLayout_8->addWidget(grip, 0, Qt::AlignBottom | Qt::AlignRight);


    // Status bar notification icons
    QFrame *frameBlocks = new QFrame();
    frameBlocks->setContentsMargins(0,0,0,0);
    frameBlocks->setMinimumWidth(56);
    frameBlocks->setMaximumWidth(56);
    QHBoxLayout *frameBlocksLayout = new QHBoxLayout(frameBlocks);
    frameBlocksLayout->setContentsMargins(3,0,3,0);
    frameBlocksLayout->setSpacing(3);
    labelEncryptionIcon = ui->label_encryption;
    labelStakingIcon = ui->label_staking;
    labelConnectionsIcon = ui->label_connections;
    labelBlocksIcon = ui->label_synced;

    if (GetBoolArg("-staking", true))
    {
        QTimer *timerStakingIcon = new QTimer(labelStakingIcon);
        connect(timerStakingIcon, SIGNAL(timeout()), this, SLOT(updateStakingIcon()));
        timerStakingIcon->start(30 * 1000);
        updateStakingIcon();
    }

    // Progress bar and label for blocks download
    progressBarLabel = ui->label_synchronization;
    progressBarLabel->setVisible(false);
    ui->label_blocks->setVisible(false);
    progressBar = ui->progressBar;
    progressBar->setAlignment(Qt::AlignCenter);
    progressBar->setVisible(false);

    // Override style sheet for progress bar for styles that have a segmented progress bar,
    // as they make the text unreadable (workaround for issue #1071)
    // See https://qt-project.org/doc/qt-4.8/gallery.html
    QString curStyle = QApplication::style()->metaObject()->className();

    syncIconMovie = new QMovie(":/movies/update_spinner", "mng", this);

    // Clicking on a transaction on the overview page simply sends you to transaction history page
    connect(overviewPage, SIGNAL(transactionClicked(QModelIndex)), this, SLOT(gotoHistoryPage()));
    connect(overviewPage, SIGNAL(transactionClicked(QModelIndex)), transactionView, SLOT(focusTransaction(QModelIndex)));

    // Double-clicking on a transaction on the transaction history page shows details
    connect(transactionView, SIGNAL(doubleClicked(QModelIndex)), transactionView, SLOT(showDetails()));

    rpcConsole = new RPCConsole(this);
    connect(openRPCConsoleAction, SIGNAL(triggered()), rpcConsole, SLOT(show()));

    // Clicking on "Verify Message" in the address book sends you to the verify message tab
    connect(addressBookPage, SIGNAL(verifyMessage(QString)), this, SLOT(gotoVerifyMessageTab(QString)));
    // Clicking on "Sign Message" in the receive coins page sends you to the sign message tab
    connect(receiveCoinsPage, SIGNAL(signMessage(QString)), this, SLOT(gotoSignMessageTab(QString)));

    // Install event filter to be able to catch status tip events (QEvent::StatusTip)
    this->installEventFilter(this);
    ui->pushButton_Overview->installEventFilter(this);
    gotoOverviewPage();
}

TrianglesGUI::~TrianglesGUI()
{
    if(trayIcon) // Hide tray icon, as deleting will let it linger until quit (on Ubuntu)
        trayIcon->hide();
#ifdef Q_OS_MAC
    delete appMenuBar;
#endif
    delete ui;
}

void TrianglesGUI::createActions(bool fIsTestnet)
{
    QActionGroup *tabGroup = new QActionGroup(this);

    overviewAction = new QAction(QIcon(":/menu_16/overview"), tr("&Overview"), this);
    overviewAction->setStatusTip(tr("Show general overview of wallet"));
    overviewAction->setToolTip(overviewAction->statusTip());
    overviewAction->setCheckable(true);
    tabGroup->addAction(overviewAction);

    sendCoinsAction = new QAction(QIcon(":/menu_16/send"), tr("&Send Triangles"), this);
    sendCoinsAction->setStatusTip(tr("Send coins to a Triangles address"));
    sendCoinsAction->setToolTip(sendCoinsAction->statusTip());
    sendCoinsAction->setCheckable(true);
    tabGroup->addAction(sendCoinsAction);

    receiveCoinsAction = new QAction(QIcon(":/menu_16/receive"), tr("&Receive Triangles"), this);
    receiveCoinsAction->setStatusTip(tr("Show the list of addresses for receiving payments"));
    receiveCoinsAction->setToolTip(receiveCoinsAction->statusTip());
    receiveCoinsAction->setCheckable(true);
    tabGroup->addAction(receiveCoinsAction);

    historyAction = new QAction(QIcon(":/menu_16/transactions"), tr("&Transactions"), this);
    historyAction->setStatusTip(tr("Browse transaction history"));
    historyAction->setToolTip(historyAction->statusTip());
    historyAction->setCheckable(true);
    tabGroup->addAction(historyAction);

    addressBookAction = new QAction(QIcon(":/menu_16/addressbook"), tr("&Address Book"), this);
    addressBookAction->setStatusTip(tr("Edit the list of stored addresses and labels"));
    addressBookAction->setToolTip(addressBookAction->statusTip());
    addressBookAction->setCheckable(true);
    tabGroup->addAction(addressBookAction);

    messageAction = new QAction(QIcon(":/menu_16/edit"), tr("&Messages"), this);
    messageAction->setToolTip(tr("View and Send Encrypted messages"));
    messageAction->setCheckable(true);
    tabGroup->addAction(messageAction);
    connect(overviewAction, SIGNAL(triggered()), this, SLOT(showNormalIfMinimized()));
    connect(overviewAction, SIGNAL(triggered()), this, SLOT(gotoOverviewPage()));
    connect(sendCoinsAction, SIGNAL(triggered()), this, SLOT(showNormalIfMinimized()));
    connect(sendCoinsAction, SIGNAL(triggered()), this, SLOT(gotoSendCoinsPage()));
    connect(receiveCoinsAction, SIGNAL(triggered()), this, SLOT(showNormalIfMinimized()));
    connect(receiveCoinsAction, SIGNAL(triggered()), this, SLOT(gotoReceiveCoinsPage()));
    connect(historyAction, SIGNAL(triggered()), this, SLOT(showNormalIfMinimized()));
    connect(historyAction, SIGNAL(triggered()), this, SLOT(gotoHistoryPage()));
    connect(addressBookAction, SIGNAL(triggered()), this, SLOT(showNormalIfMinimized()));
    connect(addressBookAction, SIGNAL(triggered()), this, SLOT(gotoAddressBookPage()));
    connect(messageAction, SIGNAL(triggered()), this, SLOT(showNormalIfMinimized()));
    connect(messageAction, SIGNAL(triggered()), this, SLOT(gotoMessagePage()));

    quitAction = new QAction(QIcon(":/menu_16/exit"), tr("E&xit Triangles wallet"), this);
    quitAction->setStatusTip(tr("Quit application"));
    quitAction->setMenuRole(QAction::QuitRole);
    if (!fIsTestnet)
        aboutAction = new QAction(QIcon(":/menu_16/triangles_menu"), tr("&About Triangles"), this);
    else
        aboutAction = new QAction(QIcon(":/icons/triangles_testnet"), tr("&About Triangles"), this);
    aboutAction->setStatusTip(tr("Show information about Triangles"));
    aboutAction->setMenuRole(QAction::AboutRole);
    aboutQtAction = new QAction(QIcon(":/menu_16/triangles_menu"), tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show information about Qt"));
    aboutQtAction->setMenuRole(QAction::AboutQtRole);
    optionsAction = new QAction(QIcon(":/menu_16/options"), tr("&Options..."), this);
    optionsAction->setStatusTip(tr("Modify configuration options for Triangles"));
    optionsAction->setMenuRole(QAction::PreferencesRole);
    if (!fIsTestnet)
        toggleHideAction = new QAction(QIcon(":/menu_16/triangles_menu"), tr("&Show / Hide Triangles wallet"), this);
    else
        toggleHideAction = new QAction(QIcon(":/icons/triangles_testnet"), tr("&Show / Hide Triangles wallet"), this);
    toggleHideAction->setStatusTip(tr("Show or hide the main Window"));

    encryptWalletAction = new QAction(QIcon(":/menu_16/lock"), tr("&Encrypt Wallet..."), this);
    encryptWalletAction->setStatusTip(tr("Encrypt the private keys that belong to your wallet"));
    encryptWalletAction->setCheckable(true);
    backupWalletAction = new QAction(QIcon(":/menu_16/export"), tr("&Backup Wallet..."), this);
    backupWalletAction->setStatusTip(tr("Backup wallet to another location"));
    changePassphraseAction = new QAction(QIcon(":/menu_16/passphrase"), tr("&Change Passphrase..."), this);
    changePassphraseAction->setStatusTip(tr("Change the passphrase used for wallet encryption"));
    unlockWalletAction = new QAction(QIcon(":/menu_16/unlock"), tr("&Unlock Wallet..."), this);
    unlockWalletAction->setStatusTip(tr("Unlock wallet"));
    unlockWalletStakingAction = new QAction(QIcon(":/menu_16/unlock"), tr("&Unlock Wallet for staking..."), this);
    unlockWalletStakingAction->setStatusTip(tr("Unlock wallet for staking"));
    lockWalletAction = new QAction(QIcon(":/menu_16/lock"), tr("&Lock Wallet"), this);
    lockWalletAction->setStatusTip(tr("Lock wallet"));
    signMessageAction = new QAction(QIcon(":/menu_16/sign"), tr("Sign &message..."), this);
    signMessageAction->setStatusTip(tr("Sign messages with your Triangles addresses to prove you own them"));
    verifyMessageAction = new QAction(QIcon(":/menu_16/verify"), tr("&Verify message..."), this);
    verifyMessageAction->setStatusTip(tr("Verify messages to ensure they were signed with specified Triangles addresses"));

    exportAction = new QAction(QIcon(":/menu_16/export"), tr("&Export..."), this);
    exportAction->setStatusTip(tr("Export the data in the current tab to a file"));
    exportAction->setToolTip(exportAction->statusTip());
    openRPCConsoleAction = new QAction(QIcon(":/menu_16/debug"), tr("&Debug window"), this);
    openRPCConsoleAction->setStatusTip(tr("Open debugging and diagnostic console"));

    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(aboutClicked()));    
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(optionsAction, SIGNAL(triggered()), this, SLOT(optionsClicked()));
    connect(toggleHideAction, SIGNAL(triggered()), this, SLOT(toggleHidden()));
    connect(encryptWalletAction, SIGNAL(triggered(bool)), this, SLOT(encryptWallet(bool)));
    connect(backupWalletAction, SIGNAL(triggered()), this, SLOT(backupWallet()));
    connect(changePassphraseAction, SIGNAL(triggered()), this, SLOT(changePassphrase()));
    connect(unlockWalletAction, SIGNAL(triggered()), this, SLOT(unlockWallet()));
    connect(unlockWalletStakingAction, SIGNAL(triggered()), this, SLOT(unlockWalletStaking()));
    connect(lockWalletAction, SIGNAL(triggered()), this, SLOT(lockWallet()));
    connect(signMessageAction, SIGNAL(triggered()), this, SLOT(gotoSignMessageTab()));
    connect(verifyMessageAction, SIGNAL(triggered()), this, SLOT(gotoVerifyMessageTab()));
}

void TrianglesGUI::createMenuBar()
{
#ifdef Q_OS_MAC
    // Create a decoupled menu bar on Mac which stays even if the window is closed
    appMenuBar = new QMenuBar();
#else
    // Get the main window's menu bar on other platforms
    appMenuBar = menuBar();
#endif

    // Configure the menus
    QMenu *file = appMenuBar->addMenu(tr("&File"));
    file->addAction(backupWalletAction);
    file->addAction(exportAction);
    file->addAction(signMessageAction);
    file->addAction(verifyMessageAction);
    file->addSeparator();
    file->addAction(quitAction);

    QMenu *settings = appMenuBar->addMenu(tr("&Settings"));
    settings->addAction(encryptWalletAction);
    settings->addAction(changePassphraseAction);
    settings->addAction(unlockWalletAction);
    settings->addAction(lockWalletAction);
    settings->addSeparator();
    settings->addAction(optionsAction);

    QMenu *help = appMenuBar->addMenu(tr("&Help"));
    help->addAction(openRPCConsoleAction);
    help->addSeparator();
    help->addAction(aboutAction);
    help->addAction(aboutQtAction);
}

void TrianglesGUI::createToolBars()
{
    QToolBar *toolbar = addToolBar(tr("Tabs toolbar"));
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbar->addAction(overviewAction);
    toolbar->addAction(sendCoinsAction);
    toolbar->addAction(receiveCoinsAction);
    toolbar->addAction(historyAction);
    toolbar->addAction(addressBookAction);
    toolbar->addAction(messageAction);

    QToolBar *toolbar2 = addToolBar(tr("Actions toolbar"));
    toolbar2->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbar2->addAction(exportAction);
}

void TrianglesGUI::setClientModel(ClientModel *clientModel)
{
    this->clientModel = clientModel;
    if(clientModel)
    {
        // Create system tray menu (or setup the dock menu) that late to prevent users from calling actions,
        // while the client has not yet fully loaded
        createTrayIconMenu();

        // Keep up to date with client
        setNumConnections(clientModel->getNumConnections());
        connect(clientModel, SIGNAL(numConnectionsChanged(int)), this, SLOT(setNumConnections(int)));

        setNumBlocks(clientModel->getNumBlocks(), clientModel->getNumBlocksOfPeers());
        connect(clientModel, SIGNAL(numBlocksChanged(int,int)), this, SLOT(setNumBlocks(int,int)));

        // Receive and report messages from network/worker thread
        connect(clientModel, SIGNAL(message(QString,QString,unsigned int)), this, SLOT(message(QString,QString,unsigned int)));

        rpcConsole->setClientModel(clientModel);
        addressBookPage->setOptionsModel(clientModel->getOptionsModel());
        receiveCoinsPage->setOptionsModel(clientModel->getOptionsModel());
    }
}

void TrianglesGUI::setWalletModel(WalletModel *walletModel)
{
    this->walletModel = walletModel;
    if(walletModel)
    {
        // Receive and report messages from wallet thread
        connect(walletModel, SIGNAL(message(QString,QString,unsigned int)), this, SLOT(message(QString,QString,unsigned int)));

        // Put transaction list in tabs
        transactionView->setModel(walletModel);
        overviewPage->setModel(walletModel);
        addressBookPage->setModel(walletModel->getAddressTableModel());
        receiveCoinsPage->setModel(walletModel->getAddressTableModel());
        sendCoinsPage->setModel(walletModel);
        signMessagePage->setModel(walletModel);
        verifyMessagePage->setModel(walletModel);

        setEncryptionStatus(walletModel->getEncryptionStatus());
        connect(walletModel, SIGNAL(encryptionStatusChanged(int)), this, SLOT(setEncryptionStatus(int)));

        // Balloon pop-up for new transaction
        connect(walletModel->getTransactionTableModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(incomingTransaction(QModelIndex,int,int)));

        // Ask for passphrase if needed
        connect(walletModel, SIGNAL(requireUnlock()), this, SLOT(unlockWallet()));
    }
}

void TrianglesGUI::setMessageModel(MessageModel *messageModel)
{
    this->messageModel = messageModel;
    if(messageModel)
    {
        // Report errors from message thread
        connect(messageModel, SIGNAL(error(QString,QString,bool)), this, SLOT(error(QString,QString,bool)));

        // Put transaction list in tabs
        messagePage->setModel(messageModel);

        // Balloon pop-up for new message
        connect(messageModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(incomingMessage(QModelIndex,int,int)));
    }
}

void TrianglesGUI::createTrayIcon()
{
#ifndef Q_OS_MAC
    trayIcon = new QSystemTrayIcon(this);

    trayIcon->setToolTip(tr("Triangles client"));
    trayIcon->setIcon(QIcon(":/icons/toolbar"));
    trayIcon->show();
#endif

    notificator = new Notificator(QApplication::applicationName(), trayIcon);
}

void TrianglesGUI::createTrayIconMenu()
{
    QMenu *trayIconMenu;
#ifndef Q_OS_MAC
    // return if trayIcon is unset (only on non-Mac OSes)
    if (!trayIcon)
        return;

    trayIconMenu = new QMenu(this);
    trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
#else
    // Note: On Mac, the dock icon is used to provide the tray's functionality.
    MacDockIconHandler *dockIconHandler = MacDockIconHandler::instance();
    dockIconHandler->setMainWindow((QMainWindow *)this);
    trayIconMenu = dockIconHandler->dockMenu();
#endif

    // Configuration of the tray icon (or dock icon) icon menu
    trayIconMenu->addAction(toggleHideAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(sendCoinsAction);
    trayIconMenu->addAction(receiveCoinsAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(signMessageAction);
    trayIconMenu->addAction(verifyMessageAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(optionsAction);
#ifndef Q_OS_MAC // This is built-in on Mac
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
#endif
}

#ifndef Q_OS_MAC
void TrianglesGUI::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Trigger)
    {
        // Click on system tray icon triggers show/hide of the main window
        toggleHideAction->trigger();
    }
}
#endif

void TrianglesGUI::saveWindowGeometry()
{
    QSettings settings;
    settings.setValue("nWindowPos", pos());
    settings.setValue("nWindowSize", size());
}

void TrianglesGUI::restoreWindowGeometry()
{
    QSettings settings;
    QPoint pos = settings.value("nWindowPos").toPoint();
    QSize size = settings.value("nWindowSize", QSize(850, 550)).toSize();
    if (!pos.x() && !pos.y())
    {
        QRect screen = QApplication::desktop()->screenGeometry();
        pos.setX((screen.width()-size.width())/2);
        pos.setY((screen.height()-size.height())/2);
    }
    resize(size);
    move(pos);
}

void TrianglesGUI::optionsClicked()
{
    if(!clientModel || !clientModel->getOptionsModel())
        return;
    OptionsDialog dlg(this);
    dlg.setModel(clientModel->getOptionsModel());
    dlg.exec();
}

void TrianglesGUI::aboutClicked()
{
    AboutDialog dlg(this);
    dlg.setModel(clientModel);
    dlg.exec();
}

void TrianglesGUI::setNumConnections(int count)
{
    QString icon;
    switch(count)
    {
    case 0: icon = ":/icons/connect_0"; break;
    case 1: case 2: case 3: icon = ":/icons/connect_1"; break;
    case 4: case 5: case 6: icon = ":/icons/connect_2"; break;
    case 7: case 8: case 9: icon = ":/icons/connect_3"; break;
    default: icon = ":/icons/connect_4"; break;
    }
    labelConnectionsIcon->setPixmap(QIcon(icon).pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
    labelConnectionsIcon->setToolTip(tr("%n active connection(s) to Triangles network", "", count));
}

void TrianglesGUI::setNumBlocks(int count, int nTotalBlocks)
{
    // don't show / hide progress bar and its label if we have no connection to the network
    if (!clientModel || clientModel->getNumConnections() == 0)
    {
        progressBarLabel->setVisible(false);
        progressBar->setVisible(false);
        ui->label_blocks->setVisible(false);

        return;
    }

    QString tooltip;

    QString importText;
    importText = tr("Synchronizing with network...");

    if(count < nTotalBlocks)
    {
        int nRemainingBlocks = nTotalBlocks - count;
        float nPercentageDone = count / (nTotalBlocks * 0.01f);

        progressBarLabel->setText(importText);
        progressBarLabel->setVisible(true);
        progressBar->setFormat(tr("~%n block(s) remaining", "", nRemainingBlocks));
        progressBar->setMaximum(nTotalBlocks);
        progressBar->setValue(count);
        progressBar->setVisible(true);
        ui->label_blocks->setText(tr("%n blocks", "", count));
        ui->label_blocks->setVisible(true);

		tooltip = tr("Downloaded %1 of %2 blocks of transaction history (%3% done).").arg(count).arg(nTotalBlocks).arg(nPercentageDone, 0, 'f', 2);
    }
    else
    {
        progressBarLabel->setVisible(false);

        progressBar->setVisible(false);
        ui->label_blocks->setVisible(false);
        tooltip = tr("Processed %1 blocks of transaction history.").arg(count);
    }
    tooltip = tr("Current difficulty is %1.").arg(clientModel->GetDifficulty()) + QString("<br>") + tooltip;

    QDateTime lastBlockDate = clientModel->getLastBlockDate();
    int secs = lastBlockDate.secsTo(QDateTime::currentDateTime());
    QString text;

    // Represent time from last generated block in human readable text
    if(secs <= 0)
    {
        // Fully up to date. Leave text empty.
    }
    else if(secs < 60)
    {
        text = tr("%n second(s) ago","",secs);
    }
    else if(secs < 60*60)
    {
        text = tr("%n minute(s) ago","",secs/60);
    }
    else if(secs < 24*60*60)
    {
        text = tr("%n hour(s) ago","",secs/(60*60));
    }
    else
    {
        text = tr("%n day(s) ago","",secs/(60*60*24));
    }

    // Set icon state: spinning if catching up, tick otherwise
    if(secs < 90*60 && count >= nTotalBlocks)
    {
        tooltip = tr("Up to date") + QString(".<br>") + tooltip;
        labelBlocksIcon->setPixmap(QIcon(":/icons/synced").pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
        
        overviewPage->showOutOfSyncWarning(false);
    }
    else
    {
        tooltip = tr("Catching up...") + QString("<br>") + tooltip;
        //syncIconMovie doesn't work for some reason - using fallback png
        //labelBlocksIcon->setMovie(syncIconMovie);
        //syncIconMovie->start();
        labelBlocksIcon->setPixmap(QIcon(":/icons/notsynced").pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));

        overviewPage->showOutOfSyncWarning(true);
    }

    if(!text.isEmpty())
    {
        tooltip += QString("<br>");
        tooltip += tr("Last received block was generated %1.").arg(text);
    }

    // Don't word-wrap this (fixed-width) tooltip
    tooltip = QString("<nobr>") + tooltip + QString("</nobr>");

    labelBlocksIcon->setToolTip(tooltip);
    progressBarLabel->setToolTip(tooltip);
    progressBar->setToolTip(tooltip);
}

void TrianglesGUI::error(const QString &title, const QString &message, bool modal)
{
    // Report errors from network/worker thread
    if(modal)
    {
        QMessageBox::critical(this, title, message, QMessageBox::Ok, QMessageBox::Ok);
    } else {
        notificator->notify(Notificator::Critical, title, message);
    }
}

void TrianglesGUI::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
#ifndef Q_OS_MAC // Ignored on Mac
    if(e->type() == QEvent::WindowStateChange)
    {
        if(clientModel && clientModel->getOptionsModel()->getMinimizeToTray())
        {
            QWindowStateChangeEvent *wsevt = static_cast<QWindowStateChangeEvent*>(e);
            if(!(wsevt->oldState() & Qt::WindowMinimized) && isMinimized())
            {
                QTimer::singleShot(0, this, SLOT(hide()));
                e->ignore();
            }
        }
    }
#endif
}

void TrianglesGUI::closeEvent(QCloseEvent *event)
{
    if(clientModel)
    {
#ifndef Q_OS_MAC // Ignored on Mac
        if(!clientModel->getOptionsModel()->getMinimizeToTray() &&
           !clientModel->getOptionsModel()->getMinimizeOnClose())
        {
            QApplication::quit();
        }
#endif
    }
    QMainWindow::closeEvent(event);
}

void TrianglesGUI::askFee(qint64 nFeeRequired, bool *payFee)
{
    QString strMessage = tr("<font color='#f26522'>This transaction is over the size limit.  You can still send it for a fee of %1, "
          "which goes to the nodes that process your transaction and helps to support the network.  </font>"
          "Do you want to pay the fee?").arg(TrianglesUnits::formatWithUnit(TrianglesUnits::TRI, nFeeRequired));
          
    QMessageBox* msgBox = new QMessageBox(QMessageBox::Question,
                                          tr("Confirm transaction fee"),
                                          strMessage,
                                          QMessageBox::Yes | QMessageBox::Cancel, this,
                                          Qt::FramelessWindowHint);
                                          
    msgBox->setIconPixmap(QPixmap(":/msgbox/question"));
    msgBox->setStyleSheet("\
                          QMessageBox {border: 2px solid #f26522;\
                                       background-color: #000;}\
                          ");
    msgBox->button(QMessageBox::Yes)->setStyleSheet("\
                          QMessageBox QPushButton {background-color: #000;color: #f26522;border: 1px solid #f26522;\
                              min-width: 120px;max-width: 120px;max-height: 20px;min-height: 20px;}\
                          QMessageBox QPushButton:hover {background-color: #61280E;}\
                          QMessageBox QPushButton:pressed:flat {color: #000;background-color: #f26522;}\
                          ");
                          
    msgBox->button(QMessageBox::Cancel)->setStyleSheet("\
                          QMessageBox QPushButton {background-color: #000;color: #f26522;border: 1px solid #f26522;\
                              min-width: 120px;max-width: 120px;max-height: 20px;min-height: 20px;}\
                          QMessageBox QPushButton:hover {background-color: #61280E;}\
                          QMessageBox QPushButton:pressed:flat {color: #000;background-color: #f26522;}\
                          ");
                                                                                                              
    int retval = msgBox->exec();                                      
                                          
    *payFee = (retval == QMessageBox::Yes);
}


void TrianglesGUI::incomingTransaction(const QModelIndex & parent, int start, int end)
{
    if(!walletModel || !clientModel)
        return;
    TransactionTableModel *ttm = walletModel->getTransactionTableModel();
    qint64 amount = ttm->index(start, TransactionTableModel::Amount, parent)
                    .data(Qt::EditRole).toULongLong();
    if(!clientModel->inInitialBlockDownload())
    {
        // On new transaction, make an info balloon
        // Unless the initial block download is in progress, to prevent balloon-spam
        QString date = ttm->index(start, TransactionTableModel::Date, parent)
                        .data().toString();
        QString type = ttm->index(start, TransactionTableModel::Type, parent)
                        .data().toString();
        QString address = ttm->index(start, TransactionTableModel::ToAddress, parent)
                        .data().toString();
        QIcon icon = qvariant_cast<QIcon>(ttm->index(start,
                            TransactionTableModel::ToAddress, parent)
                        .data(Qt::DecorationRole));

        notificator->notify(Notificator::Information,
                            (amount)<0 ? tr("Sent transaction") :
                                         tr("Incoming transaction"),
                              tr("Date: %1\n"
                                 "Amount: %2\n"
                                 "Type: %3\n"
                                 "Address: %4\n")
                              .arg(date)
                              .arg(TrianglesUnits::formatWithUnit(walletModel->getOptionsModel()->getDisplayUnit(), amount, true))
                              .arg(type)
                              .arg(address), icon);
    }
}

void TrianglesGUI::incomingMessage(const QModelIndex & parent, int start, int end)
{
    if(!messageModel)
        return;

    MessageModel *mm = messageModel;

    if (mm->index(start, MessageModel::TypeInt, parent).data().toInt() == MessageTableEntry::Received)
    {
        QString sent_datetime = mm->index(start, MessageModel::ReceivedDateTime, parent).data().toString();
        QString from_address  = mm->index(start, MessageModel::FromAddress,      parent).data().toString();
        QString to_address    = mm->index(start, MessageModel::ToAddress,        parent).data().toString();
        QString message       = mm->index(start, MessageModel::Message,          parent).data().toString();
        QTextDocument html;
        html.setHtml(message);
        QString messageText(html.toPlainText());
        notificator->notify(Notificator::Information,
                            tr("Incoming Message"),
                            tr("Date: %1\n"
                               "From Address: %2\n"
                               "To Address: %3\n"
                               "Message: %4\n")
                              .arg(sent_datetime)
                              .arg(from_address)
                              .arg(to_address)
                              .arg(messageText));
    };
}

void TrianglesGUI::gotoOverviewPage()
{
    overviewAction->setChecked(true);
    centralWidget->setCurrentWidget(overviewPage);

    exportAction->setEnabled(false);
    disconnect(exportAction, SIGNAL(triggered()), 0, 0);
}

void TrianglesGUI::gotoHistoryPage()
{
    historyAction->setChecked(true);
    centralWidget->setCurrentWidget(transactionsPage);

    exportAction->setEnabled(true);
    disconnect(exportAction, SIGNAL(triggered()), 0, 0);
    connect(exportAction, SIGNAL(triggered()), transactionsPage, SLOT(exportClicked()));
}

void TrianglesGUI::gotoAddressBookPage()
{
    addressBookAction->setChecked(true);
    centralWidget->setCurrentWidget(addressBookPage);

    exportAction->setEnabled(true);
    disconnect(exportAction, SIGNAL(triggered()), 0, 0);
    connect(exportAction, SIGNAL(triggered()), addressBookPage, SLOT(exportClicked()));
}

void TrianglesGUI::gotoReceiveCoinsPage()
{
    receiveCoinsAction->setChecked(true);
    centralWidget->setCurrentWidget(receiveCoinsPage);

    exportAction->setEnabled(true);
    disconnect(exportAction, SIGNAL(triggered()), 0, 0);
    connect(exportAction, SIGNAL(triggered()), receiveCoinsPage, SLOT(exportClicked()));
}

void TrianglesGUI::gotoSendCoinsPage()
{
    sendCoinsAction->setChecked(true);
    centralWidget->setCurrentWidget(sendCoinsPage);

    exportAction->setEnabled(false);
    disconnect(exportAction, SIGNAL(triggered()), 0, 0);	
}

void TrianglesGUI::gotoMessagePage()
{
    messageAction->setChecked(true);
    centralWidget->setCurrentWidget(messagePage);

    exportAction->setEnabled(true);
    disconnect(exportAction, SIGNAL(triggered()), 0, 0);
    connect(exportAction, SIGNAL(triggered()), messagePage, SLOT(exportClicked()));
}

void TrianglesGUI::gotoSignMessageTab(QString addr)
{
    centralWidget->setCurrentWidget(signMessagePage);

    exportAction->setEnabled(false);
    disconnect(exportAction, SIGNAL(triggered()), 0, 0);

    // call show() in showTab_SM()
    signMessagePage->showTab_SM(true);

    if(!addr.isEmpty())
        signMessagePage->setAddress_SM(addr);
}

void TrianglesGUI::gotoVerifyMessageTab(QString addr)
{
    centralWidget->setCurrentWidget(verifyMessagePage);

    exportAction->setEnabled(false);
    disconnect(exportAction, SIGNAL(triggered()), 0, 0);

    // call show() in showTab_VM()
    verifyMessagePage->showTab_VM(true);

    if(!addr.isEmpty())
        verifyMessagePage->setAddress_VM(addr);
}

void TrianglesGUI::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept only URIs
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void TrianglesGUI::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        int nValidUrisFound = 0;
        QList<QUrl> uris = event->mimeData()->urls();
        foreach(const QUrl &uri, uris)
        {
	    if (sendCoinsPage->handleURI(uri.toString()))
                nValidUrisFound++;
        }

        // if valid URIs were found
        if (nValidUrisFound)
		    gotoSendCoinsPage();
        else
            notificator->notify(Notificator::Warning, tr("URI handling"), tr("URI can not be parsed! This can be caused by an invalid TRI address or malformed URI parameters."));
    }

    event->acceptProposedAction();
}

bool TrianglesGUI::eventFilter(QObject *object, QEvent *event)
{
    // Catch status tip events
    if (event->type() == QEvent::StatusTip)
    {
        // Prevent adding text from setStatusTip(), if we currently use the status bar for displaying other stuff
        if (progressBarLabel->isVisible() || progressBar->isVisible())
            return true;
    }
    if (object == ui->pushButton_Overview && event->type() == QEvent::MouseButtonPress)
        gotoOverviewPage();
    return QMainWindow::eventFilter(object, event);
}

void TrianglesGUI::resizeEvent(QResizeEvent *e)
{
    updateMask();
    QMainWindow::resizeEvent(e);
}

void TrianglesGUI::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    updateMask();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // we need this in order to get correct rounded corners
    painter.setPen(QPen(QBrush(Qt::black), 2.0));
    painter.setBrush(QBrush(QColor(0, 140, 0)));

    QMainWindow::paintEvent(e);
}

void TrianglesGUI::updateMask()
{
    // main form mask
    _mask = QBitmap(size());
    _mask.clear();
    {
        QPainter painter(&_mask);
        painter.fillRect(rect(), Qt::color1);
        painter.drawImage(0, 0, QImage("://res/mask0.png"));
        painter.end();
    }
    setMask(_mask);

    // logo widget corner
    _logoWidgetMask = QBitmap(ui->wHeader->size());
    _logoWidgetMask.clear();

    {
        QPainter painter(&_logoWidgetMask);
        painter.fillRect(ui->wHeader->rect(), Qt::color1);
        painter.drawImage(0, 0, QImage("://res/mask1.png"));
        painter.end();
    }
    ui->wHeader->setMask(_logoWidgetMask);
}

void TrianglesGUI::handleURI(QString strURI)
{
    // URI has to be valid
    if (sendCoinsPage->handleURI(strURI))
    {
        showNormalIfMinimized();
        gotoSendCoinsPage();
    }
    else
        notificator->notify(Notificator::Warning, tr("URI handling"), tr("URI can not be parsed! This can be caused by an invalid TRI address or malformed URI parameters."));
}

void TrianglesGUI::menuFileRequested()
{
    QMenu menu(this);
    QAction* home = menu.addAction(QIcon(":/menu_16/overview"), tr("&Overview").remove('&'));
    QAction* commonAndNetwork = menu.addAction(QIcon(":/menu_16/options"), tr("Options"));
    QAction* qaBackupWallet = menu.addAction(QIcon(":/menu_16/backup"), tr("Backup Wallet"));
    QAction* exportData = menu.addAction(QIcon(":/menu_16/export"), tr("&Export...").remove('&').remove("..."));
    QAction* exitApp = menu.addAction(QIcon(":/menu_16/exit"), tr("E&xit").remove('&'));

    QPoint poz = QCursor::pos();
    if (QWidget* w = qobject_cast<QWidget*>(sender()))
        poz = w->mapToGlobal(w->rect().bottomLeft());

    QAction* selected = menu.exec(poz);
    if (!selected)
        return;

    if (selected == home)
    {
        gotoOverviewPage();
    }
    else if (selected == commonAndNetwork)
    {
        optionsClicked();
    }
    else if (selected == qaBackupWallet) {
        this->backupWallet();
    }
    else if (selected == exportData)
    {
        if (centralWidget->currentWidget() == transactionsPage)
        {
            transactionView->exportClicked();
        } else if (centralWidget->currentWidget() == addressBookPage)
        {
            addressBookPage->exportClicked();
        } else if (centralWidget->currentWidget() == receiveCoinsPage)
        {
            receiveCoinsPage->exportClicked();
        }
    }
    else if (selected == exitApp)
    {
        qApp->quit();
    }
}

void TrianglesGUI::menuOperationsRequested()
{
    QMenu menu(this);
    QAction* send = menu.addAction(QIcon(":/menu_16/send"), tr("Send Triangles"));
    QAction* receive = menu.addAction(QIcon(":/menu_16/receive"), tr("Receive Triangles"));
    QAction* transactions = menu.addAction(QIcon(":/menu_16/transactions"), tr("&Transactions").remove('&'));
    QAction* addressBook = menu.addAction(QIcon(":/menu_16/addressbook"), tr("&Address Book").remove('&'));
    QAction* encryptWallet = menu.addAction(QIcon(":/menu_16/lock"), tr("&Encrypt Wallet...").remove('&').remove("..."));
    QAction* unlockWalletStaking = menu.addAction(QIcon(":/menu_16/unlock"), tr("&Unlock Wallet...").remove('&').remove("..."));
    QAction* lockWallet = menu.addAction(QIcon(":/menu_16/lock"), tr("&Lock Wallet...").remove('&').remove("..."));
    QAction* changePassword = menu.addAction(QIcon(":/menu_16/passphrase"), tr("&Change Passphrase...").remove('&').remove("..."));
    QAction* signMessage = menu.addAction(QIcon(":/menu_16/sign"), tr("Sign &message...").remove('&').remove("..."));
    QAction* verifySignature = menu.addAction(QIcon(":/menu_16/verify"), tr("&Verify message...").remove('&').remove("..."));

    if (!walletModel->getEncryptionStatus() == WalletModel::Unencrypted)
        encryptWallet->setVisible(false);

    if (walletModel->getEncryptionStatus() == WalletModel::Unlocked || walletModel->getEncryptionStatus() == WalletModel::Unencrypted)
        unlockWalletStaking->setVisible(false);

    if (walletModel->getEncryptionStatus() == WalletModel::Locked || walletModel->getEncryptionStatus() == WalletModel::Unencrypted)
        lockWallet->setVisible(false);

    if (walletModel->getEncryptionStatus() == WalletModel::Unencrypted)
        changePassword->setVisible(false);


    QPoint poz = QCursor::pos();
    if (QWidget* w = qobject_cast<QWidget*>(sender()))
        poz = w->mapToGlobal(w->rect().bottomLeft());

    QAction* selected = menu.exec(poz);
    if (!selected)
        return;

    if (selected == send)
    {
        gotoSendCoinsPage();
    }
    else if (selected == receive)
    {
        gotoReceiveCoinsPage();
    }
    else if (selected == transactions)
    {
        gotoHistoryPage();
    }
    else if (selected == addressBook)
    {
        gotoAddressBookPage();
    }
    else if (selected == encryptWallet)
    {
        if (walletModel->getEncryptionStatus() == WalletModel::Unencrypted)
            this->encryptWallet(true);
    }
    else if (selected == unlockWalletStaking)
    {
        if (walletModel->getEncryptionStatus() == WalletModel::Locked)
            this->unlockWalletStaking();
    }
    else if (selected == lockWallet)
    {
        if (walletModel->getEncryptionStatus() == WalletModel::Unlocked)
            this->lockWallet();
    }
    else if (selected == changePassword)
    {
        if (walletModel->getEncryptionStatus() == WalletModel::Unlocked || walletModel->getEncryptionStatus() == WalletModel::Locked)
            changePassphrase();
    }
    else if (selected == signMessage)
    {
        gotoSignMessageTab();
    }
    else if (selected == verifySignature)
    {
        gotoVerifyMessageTab();
    }
}
void TrianglesGUI::menuSettingsRequested()
{
    QMenu menu(this);
    QAction* debugWindow = menu.addAction(QIcon(":/menu_16/debug"),tr("&Debug window").remove('&'));
    QAction* about = menu.addAction(QIcon(":/menu_16/triangles_menu"),tr("&About Triangles").remove('&'));    

    QPoint poz = QCursor::pos();
    if (QWidget* w = qobject_cast<QWidget*>(sender()))
        poz = w->mapToGlobal(w->rect().bottomLeft());

    QAction* selected = menu.exec(poz);
    if (!selected)
        return;


    if (selected == about)
    {
        aboutClicked();
    } else if (selected == debugWindow)
    {
        on_bHelp_clicked();
    }
}
void TrianglesGUI::setEncryptionStatus(int status)
{
    switch(status)
    {
    case WalletModel::Unencrypted:
        labelEncryptionIcon->hide();
        encryptWalletAction->setChecked(false);
        changePassphraseAction->setEnabled(false);
        unlockWalletStakingAction->setVisible(false);
        lockWalletAction->setVisible(false);
        encryptWalletAction->setEnabled(true);
        break;
    case WalletModel::Unlocked:
        labelEncryptionIcon->show();
        labelEncryptionIcon->setPixmap(QIcon(":/icons/lock_open").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        if (fWalletUnlockStakingOnly) {
            labelEncryptionIcon->setToolTip(tr("Wallet is <b>encrypted</b> and currently <b>unlocked for staking</b>"));
        }
        else {
            labelEncryptionIcon->setToolTip(tr("Wallet is <b>encrypted</b> and currently <b>unlocked</b>"));
        }
        encryptWalletAction->setChecked(true);
        changePassphraseAction->setEnabled(true);
        unlockWalletStakingAction->setVisible(false);
        lockWalletAction->setVisible(true);
        encryptWalletAction->setEnabled(false); // TODO: decrypt currently not supported
        break;
    case WalletModel::Locked:
        labelEncryptionIcon->show();
        labelEncryptionIcon->setPixmap(QIcon(":/icons/lock_closed").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        labelEncryptionIcon->setToolTip(tr("Wallet is <b>encrypted</b> and currently <b>locked</b>"));
        encryptWalletAction->setChecked(true);
        changePassphraseAction->setEnabled(true);
        unlockWalletStakingAction->setVisible(true);
        lockWalletAction->setVisible(false);
        encryptWalletAction->setEnabled(false); // TODO: decrypt currently not supported
        break;
    }
}



void TrianglesGUI::encryptWallet(bool status)
{
    if(!walletModel)
        return;
    AskPassphraseDialog dlg(status ? AskPassphraseDialog::Encrypt:
                                     AskPassphraseDialog::Decrypt, this);
    dlg.setModel(walletModel);
    dlg.exec();

    setEncryptionStatus(walletModel->getEncryptionStatus());
}

void TrianglesGUI::backupWallet()
{
    QString saveDir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    QString filename = QFileDialog::getSaveFileName(this, tr("Backup Wallet"), saveDir, tr("Wallet Data (*.dat)"));
    if(!filename.isEmpty()) {
        if(!walletModel->backupWallet(filename)) {
            QMessageBox::warning(this, tr("Backup failed"), tr("There was an error trying to save the wallet data to the new location."));
        }
        else
            QMessageBox::information(this, tr("Backup successful"), tr("The wallet data was successfully saved to the new location."));
    }
}

void TrianglesGUI::changePassphrase()
{
    AskPassphraseDialog dlg(AskPassphraseDialog::ChangePass, this);
    dlg.setModel(walletModel);
    dlg.exec();
}


void TrianglesGUI::unlockWalletStaking()
{
    if(!walletModel)
        return;
    // Unlock wallet when requested by wallet model
    if(walletModel->getEncryptionStatus() == WalletModel::Locked)
    {
        AskPassphraseDialog dlg(AskPassphraseDialog::UnlockStaking, this);
        dlg.setModel(walletModel);
        dlg.exec();
    }
}



void TrianglesGUI::unlockWallet()
{
    if(!walletModel)
        return;
    // Unlock wallet when requested by wallet model
    if(walletModel->getEncryptionStatus() == WalletModel::Locked)
    {
        AskPassphraseDialog dlg(AskPassphraseDialog::Unlock, this);
        dlg.setModel(walletModel);
        dlg.exec();
    }
}

void TrianglesGUI::lockWallet()
{
    if(!walletModel)
        return;

    walletModel->setWalletLocked(true);
}

void TrianglesGUI::showNormalIfMinimized(bool fToggleHidden)
{
    // activateWindow() (sometimes) helps with keyboard focus on Windows
    if (isHidden())
    {
        show();
        activateWindow();
    }
    else if (isMinimized())
    {
        showNormal();
        activateWindow();
    }
    else if (GUIUtil::isObscured(this))
    {
        raise();
        activateWindow();
    }
    else if(fToggleHidden)
        hide();
}

void TrianglesGUI::toggleHidden()
{
    showNormalIfMinimized(true);
}


void TrianglesGUI::updateStakingIcon()
{
    uint64_t nMinWeight = 0, nMaxWeight = 0, nWeight = 0;
    if (pwalletMain)
        pwalletMain->GetStakeWeight(*pwalletMain, nMinWeight, nMaxWeight, nWeight);

    if (nLastCoinStakeSearchInterval && nWeight)
    {
        uint64_t nNetworkWeight = GetPoSKernelPS();
        unsigned nEstimateTime = nTargetSpacing * nNetworkWeight / nWeight;

        QString text;
        if (nEstimateTime < 60)
        {
            text = tr("%n second(s)", "", nEstimateTime);
        }
        else if (nEstimateTime < 60*60)
        {
            text = tr("%n minute(s)", "", nEstimateTime/60);
        }
        else if (nEstimateTime < 24*60*60)
        {
            text = tr("%n hour(s)", "", nEstimateTime/(60*60));
        }
        else
        {
            text = tr("%n day(s)", "", nEstimateTime/(60*60*24));
        }

        labelStakingIcon->setPixmap(QIcon(":/icons/staking_on").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        labelStakingIcon->setToolTip(tr("Staking.<br>Your weight is %1<br>Network weight is %2<br>Expected time to earn reward is %3").arg(nWeight).arg(nNetworkWeight).arg(text));
    }
    else
    {
        labelStakingIcon->setPixmap(QIcon(":/icons/staking_off").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        if (pwalletMain && pwalletMain->IsLocked())
            labelStakingIcon->setToolTip(tr("Not staking because wallet is locked"));
        else if (vNodes.empty())
            labelStakingIcon->setToolTip(tr("Not staking because wallet is offline"));
        else if (IsInitialBlockDownload())
            labelStakingIcon->setToolTip(tr("Not staking because wallet is syncing"));
        else if (!nWeight)
            labelStakingIcon->setToolTip(tr("Not staking because you don't have mature coins"));
        else
            labelStakingIcon->setToolTip(tr("Not staking"));
    }
}

void TrianglesGUI::detectShutdown()
{
    if (ShutdownRequested())
        QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
}


void TrianglesGUI::on_bHelp_clicked()
{    
    rpcConsole->show();
}
