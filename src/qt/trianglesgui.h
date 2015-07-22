#ifndef TRIANGLESGUI_H
#define TRIANGLESGUI_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMap>
#include <QBitmap>

class TransactionTableModel;
class ClientModel;
class WalletModel;
class MessageModel;
class TransactionView;
class OverviewPage;
class AddressBookPage;
class MessagePage;
class SendCoinsDialog;
class SignVerifyMessageDialog;
class Notificator;
class RPCConsole;
class SignMessagePage;
class VerifyMessagePage;

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QTableView;
class QAbstractItemModel;
class QModelIndex;
class QProgressBar;
class QStackedWidget;
class QUrl;
class QListWidget;
class QPushButton;
class QAction;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

/**
  Triangles GUI main class. This class represents the main window of the Triangles UI. It communicates with both the client and
  wallet models to give the user an up-to-date view of the current core state.
*/
class TrianglesGUI : public QMainWindow
{
    Q_OBJECT

public:
    static const QString DEFAULT_WALLET;

    explicit TrianglesGUI(bool fIsTestnet = false, QWidget *parent = 0);
    ~TrianglesGUI();

    /** Set the client model.
        The client model represents the part of the core that communicates with the P2P network, and is wallet-agnostic.
    */
    void setClientModel(ClientModel *clientModel);
    /** Set the wallet model.
        The wallet model represents a triangles wallet, and offers access to the list of transactions, address book and sending
        functionality.
    */
    void setWalletModel(WalletModel *walletModel);
    /** Set the message model.
        The message model represents encryption message database, and offers access to the list of messages, address book and sending
        functionality.
    */
    void setMessageModel(MessageModel *messageModel);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    bool eventFilter(QObject *object, QEvent *event);
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    void updateMask();

private:
    Ui::MainWindow *ui;
    QBitmap _mask;
    QBitmap _logoWidgetMask;

    ClientModel *clientModel;
    WalletModel *walletModel;
    MessageModel *messageModel;

    QStackedWidget *centralWidget;

    OverviewPage *overviewPage;
    QWidget *transactionsPage;
    AddressBookPage *addressBookPage;
    AddressBookPage *receiveCoinsPage;
    MessagePage *messagePage;
    SendCoinsDialog *sendCoinsPage;
    SignMessagePage* signMessagePage;
    VerifyMessagePage* verifyMessagePage;

    QLabel *labelEncryptionIcon;
    QLabel *labelStakingIcon;
    QLabel *labelConnectionsIcon;
    QLabel *labelBlocksIcon;
    QLabel *progressBarLabel;
    QProgressBar *progressBar;

    QMenuBar *appMenuBar;
    QAction *overviewAction;
    QAction *historyAction;
    QAction *quitAction;
    QAction *sendCoinsAction;
    QAction *addressBookAction;
    QAction *messageAction;
    QAction *signMessageAction;
    QAction *verifyMessageAction;
    QAction *aboutAction;
    QAction *receiveCoinsAction;
    QAction *optionsAction;
    QAction *toggleHideAction;
    QAction *exportAction;
    QAction *encryptWalletAction;
    QAction *backupWalletAction;
    QAction *changePassphraseAction;
    QAction *unlockWalletAction;
    QAction *unlockWalletStakingAction;
    QAction *lockWalletAction;
    QAction *aboutQtAction;
    QAction *openRPCConsoleAction;

    QSystemTrayIcon *trayIcon;
    Notificator *notificator;
    TransactionView *transactionView;
    RPCConsole *rpcConsole;

    QMovie *syncIconMovie;
    /** Keep track of previous number of blocks, to detect progress */
    int prevBlocks;

    /** Create the main UI actions. */
    void createActions(bool fIsTestnet);
    /** Create the menu bar and sub-menus. */
    void createMenuBar();
    /** Create the toolbars */
    void createToolBars();
    /** Create system tray icon and notification */
    void createTrayIcon();
    /** Create system tray menu (or setup the dock menu) */
    void createTrayIconMenu();
    /** Save window size and position */
    void saveWindowGeometry();
    /** Restore window size and position */
    void restoreWindowGeometry();

public slots:
    /** Set number of connections shown in the UI */
    void setNumConnections(int count);
    /** Set number of blocks shown in the UI */
    void setNumBlocks(int count, int nTotalBlocks);
    /** Set the encryption status as shown in the UI.
       @param[in] status            current encryption status
       @see WalletModel::EncryptionStatus
    */
    void setEncryptionStatus(int status);

    /** Notify the user of an error in the network or transaction handling code. */
    void error(const QString &title, const QString &message, bool modal);
    /** Asks the user whether to pay the transaction fee or to cancel the transaction.
       It is currently not possible to pass a return value to another thread through
       BlockingQueuedConnection, so an indirected pointer is used.
       https://bugreports.qt-project.org/browse/QTBUG-10440

      @param[in] nFeeRequired       the required fee
      @param[out] payFee            true to pay the fee, false to not pay the fee
    */
    void askFee(qint64 nFeeRequired, bool *payFee);
    void handleURI(QString strURI);


private slots:

    void menuFileRequested();
    void menuOperationsRequested();
    void menuSettingsRequested();

    /** Switch to overview (home) page */
    void gotoOverviewPage();
    /** Switch to history (transactions) page */
    void gotoHistoryPage();
    /** Switch to address book page */
    void gotoAddressBookPage();
    /** Switch to receive coins page */
    void gotoReceiveCoinsPage();
    /** Switch to send coins page */
    void gotoSendCoinsPage();
    /** Switch to message page */
    void gotoMessagePage();

    /** Show Sign/Verify Message dialog and switch to sign message tab */
    void gotoSignMessageTab(QString addr = "");
    /** Show Sign/Verify Message dialog and switch to verify message tab */
    void gotoVerifyMessageTab(QString addr = "");

    /** Show configuration dialog */
    void optionsClicked();
    /** Show about dialog */
    void aboutClicked();
#ifndef Q_OS_MAC
    /** Handle tray icon clicked */
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
#endif
    /** Show incoming transaction notification for new transactions.

        The new items are those between start and end inclusive, under the given parent item.
    */
    void incomingTransaction(const QModelIndex & parent, int start, int end);

    /** Show incoming message notification for new messages.

        The new items are those between start and end inclusive, under the given parent item.
    */
    void incomingMessage(const QModelIndex & parent, int start, int end);

    /** Encrypt the wallet */
    void encryptWallet(bool status);
    /** Backup the wallet */
    void backupWallet();
    /** Change encrypted wallet passphrase */
    void changePassphrase();
    /** Ask for passphrase to unlock wallet temporarily */
    void unlockWallet();
    /** Ask for passphrase to unlock wallet temporarily - FOR STAKING ONLY */
    void unlockWalletStaking();

    void lockWallet();

    /** Show window if hidden, unminimize when minimized, rise when obscured or show if hidden and fToggleHidden is true */
    void showNormalIfMinimized(bool fToggleHidden = false);
    /** simply calls showNormalIfMinimized(true) for use in SLOT() macro */
    void toggleHidden();

    void updateStakingIcon();
	/** called by a timer to check if fRequestShutdown has been set **/
	void detectShutdown();

    void on_bHelp_clicked();

};

#endif //TRIANGLESGUI_H
