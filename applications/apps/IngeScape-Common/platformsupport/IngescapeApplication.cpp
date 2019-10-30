/*
 *	IngeScape Common
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */


#include "IngescapeApplication.h"

#include <QtGlobal>
#include <QDebug>
#include <QFileInfo>
#include <QFileOpenEvent>
#include <QAbstractNativeEventFilter>


#ifdef Q_OS_WIN
#include <Windows.h>
#include <dde.h>
#endif



//---------------------------------------------------------------------
//
//
//  IngescapeApplicationPrivate - Interface
//
//
//  Based on Qt wiki page
//  https://wiki.qt.io/Assigning_a_file_type_to_an_Application_on_Windows
//
//
//---------------------------------------------------------------------

/**
 * @brief The IngescapeApplicationPrivate class defines the private API of IngescapeApplicationPrivate
 */
#ifdef Q_OS_WIN
//
// Windows
//
class IngescapeApplicationPrivate: public QAbstractNativeEventFilter
{
public:
    /**
     * @brief Constructor
     */
    IngescapeApplicationPrivate(IngescapeApplication* parent);


    /**
     * @brief Destructor
     */
    ~IngescapeApplicationPrivate();


    /**
     * @brief Enable DDE commands
     */
    void enableDdeCommands();


    /**
     * @brief Disbale DDE commands
     */
    void disableDdeCommands();


    /**
     * @brief This method is called for every native event
     * @param eventType
     * @param message
     * @param result
     * @return
     */
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;



    /**
     * @Brief Implementation of the WM_DDE_INITIATE windows message
     */
    bool ddeInitiate(MSG *message, long *result);


    /**
     * @brief Implementation of the WM_DDE_EXECUTE windows message
     */
    bool ddeExecute(MSG *message, long *result);


    /**
     * @brief Implementation of the WM_DDE_TERMINATE windows message
     */
    bool ddeTerminate(MSG *message, long *result);


    /**
     * @brief Called when we receive a DDE command
     * @param command
     * @param params
     * @return
     */
    void onDdeCommand(const QString &command, const QString &params);


    /**
     * @brief Sets specified value in the registry under HKCU\Software\Classes, which is mapped to HKCR then.
     */
    bool SetHkcrUserRegKey(QString key, const QString &value, const QString &valueName = QString::null);


    /**
     * @brief Register a file
     * @param documentId
     * @param fileTypeName
     * @param fileExtension
     * @param appIconIndex
     */
    void registerFileType(const QString &documentId, const QString &fileTypeName, const QString &fileExtension, qint32 appIconIndex = 0);


    /**
     * @brief Register a DDE command (e.g. oepen)
     * @param command
     * @param documentId
     * @param cmdLineArg
     * @param ddeCommand
     */
    void registerCommand(const QString &command, const QString &documentId, const QString cmdLineArg = QString::null, const QString ddeCommand = QString::null);


protected:
    // Pointer to our public API
    IngescapeApplication* _parent;

    // the name of the application, without file extension
    QString appAtomName;
    // the name of the system topic atom, typically "System"
    QString systemTopicAtomName;
    // The windows atom needed for DDE communication
    ATOM appAtom;
    // The windows system topic atom needed for DDE communication
    ATOM systemTopicAtom;

};


#else

//
// NonWindows
//
class IngescapeApplicationPrivate
{
public:
    /**
     * @brief Constructor
     */
    IngescapeApplicationPrivate(IngescapeApplication* parent)
    {
        Q_UNUSED(parent)
    }


    /**
     * @brief Destructor
     */
    ~IngescapeApplicationPrivate()
    {
    }
};

#endif




//---------------------------------------------------------------------
//
//
//  IngescapeApplicationPrivate - Implementation
//
//
//  Based on Qt wiki page
//  https://wiki.qt.io/Assigning_a_file_type_to_an_Application_on_Windows
//
//
//---------------------------------------------------------------------

#ifdef Q_OS_WIN
//
// Windows
//


/**
 * @brief Constructor
 * @param parent
 */
IngescapeApplicationPrivate::IngescapeApplicationPrivate(IngescapeApplication* parent)
    : QAbstractNativeEventFilter(),
      _parent(parent),
      appAtomName(QFileInfo(QApplication::applicationFilePath()).baseName()),
      systemTopicAtomName("system"),
      appAtom(0),
      systemTopicAtom(0)
{
}


/**
 * @brief Destructor
 */
IngescapeApplicationPrivate::~IngescapeApplicationPrivate()
{
    if (appAtom != 0)
    {
        ::GlobalDeleteAtom(appAtom);
       appAtom = 0;
    }

    if (systemTopicAtom != 0)
    {
        ::GlobalDeleteAtom(systemTopicAtom);
        systemTopicAtom = 0;
    }

    _parent = nullptr;
}


/**
 * @brief Enable DDE commands
 */
void IngescapeApplicationPrivate::enableDdeCommands()
{
    if ((appAtom == 0) && (systemTopicAtom == 0))
    {
#ifdef UNICODE
        appAtom = ::GlobalAddAtom( reinterpret_cast<const WCHAR *>(appAtomName.utf16()) );
#else
        auto tempAppName = appAtomName.toLocal8Bit();
        appAtom = ::GlobalAddAtom( tempAppName.constData() );
#endif

#ifdef UNICODE
        systemTopicAtom = ::GlobalAddAtom( reinterpret_cast<const WCHAR *>(systemTopicAtomName.utf16()) );
#else
        auto tempSystemTopicName = systemTopicAtomName.toLocal8Bit();
        systemTopicAtom = ::GlobalAddAtom( tempSystemTopicName.constData() );
#endif
    }
}


/**
 * @brief Disbale DDE commands
 */
void IngescapeApplicationPrivate::disableDdeCommands()
{
    if (appAtom != 0)
    {
        ::GlobalDeleteAtom(appAtom);
        appAtom = 0;
    }

    if (systemTopicAtom != 0)
    {
        ::GlobalDeleteAtom(systemTopicAtom);
        systemTopicAtom = 0;
    }
}


/**
 * @brief This method is called for every native event
 * @param eventType
 * @param message
 * @param result
 * @return
 */
bool IngescapeApplicationPrivate::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    bool eventFiltered = false;

    if (eventType == "windows_generic_MSG")
    {
        MSG* msg = static_cast<MSG*>(message);
        if (msg != nullptr)
        {
            switch (msg->message)
            {
                case WM_DDE_INITIATE:
                {
                    eventFiltered = ddeInitiate(msg, result);
                }
                break;

                case WM_DDE_EXECUTE:
                {
                    eventFiltered = ddeExecute(msg, result);
                }
                break;

                case WM_DDE_TERMINATE:
                {
                    eventFiltered = ddeTerminate(msg, result);
                }
                break;

                default:
                    break;
            }
        }
    }

    return eventFiltered;
}



/**
 * @Brief Implementation of the WM_DDE_INITIATE windows message
 */
bool IngescapeApplicationPrivate::ddeInitiate(MSG *message, long *result)
{
    bool eventFiltered = false;

    if (
        ((_parent != nullptr) && (_parent->currentWindow() != nullptr))
        &&
        (0 != LOWORD(message->lParam))
        &&
        (0 != HIWORD(message->lParam))
        &&
        (LOWORD(message->lParam) == appAtom)
        &&
        (HIWORD(message->lParam) == systemTopicAtom)
        )
    {
        WId winId = _parent->currentWindow()->winId();

        // make duplicates of the incoming atoms (really adding a reference)
        TCHAR atomName[_MAX_PATH];

        bool ok;
        ok = (::GlobalGetAtomName(appAtom, atomName, _MAX_PATH-1) != 0);

        if (ok)
        {
            ok = (::GlobalAddAtom(atomName) == appAtom);
        }

        if (ok)
        {
            ok = (::GlobalGetAtomName(systemTopicAtom, atomName, _MAX_PATH-1) != 0);
        }

        if (ok)
        {
            ok = (::GlobalAddAtom(atomName) == systemTopicAtom);
        }

        if (ok)
        {
            // send the WM_DDE_ACK (caller will delete duplicate atoms)
            ::SendMessage(
                          reinterpret_cast<HWND>(message->wParam),
                          WM_DDE_ACK,
                          static_cast<WPARAM>(winId),
                          MAKELPARAM(appAtom, systemTopicAtom)
                          );

            if (result) *result = 0;

            eventFiltered = true;
        }
    }

    return eventFiltered;
}


/**
 * @brief Implementation of the WM_DDE_EXECUTE windows message
 */
bool IngescapeApplicationPrivate::ddeExecute(MSG *message, long *result)
{
    bool eventFiltered = false;

    if ((_parent != nullptr) && (_parent->currentWindow() != nullptr))
    {

        WId winId = _parent->currentWindow()->winId();

        // unpack the DDE message
        UINT_PTR unused;
        HGLOBAL hData;

        //IA64: Assume DDE LPARAMs are still 32-bit
        bool ok = (::UnpackDDElParam(WM_DDE_EXECUTE, message->lParam, &unused, (UINT_PTR*)&hData) != 0);

        if (ok)
        {
            QString command = QString::fromWCharArray((LPCWSTR)::GlobalLock(hData));
            ::GlobalUnlock(hData);

            // acknowledge now - before attempting to execute
            ::PostMessage(
                          reinterpret_cast<HWND>(message->wParam),
                          WM_DDE_ACK,
                          static_cast<WPARAM>(winId),
                          //IA64: Assume DDE LPARAMs are still 32-bit
                          ReuseDDElParam(message->lParam, WM_DDE_EXECUTE, WM_DDE_ACK, 0x8000, reinterpret_cast<UINT_PTR>(hData))
                          );

            // Check if we have a valid command
            QRegExp regCommand("^\\[(\\w+)\\((.*)\\)\\]$");
            if (regCommand.exactMatch(command))
            {
                onDdeCommand(regCommand.cap(1), regCommand.cap(2));
            }

            if (result) *result = 0;

            eventFiltered = true;
        }
    }

    return eventFiltered;
}


/**
 * @brief Implementation of the WM_DDE_TERMINATE windows message
 */
bool IngescapeApplicationPrivate::ddeTerminate(MSG *message, long *result)
{
    bool eventFiltered = false;

    if ((_parent != nullptr) && (_parent->currentWindow() != nullptr))
    {
        WId winId = _parent->currentWindow()->winId();

        ::PostMessage(reinterpret_cast<HWND>(message->wParam), WM_DDE_TERMINATE, static_cast<WPARAM>(winId), message->lParam);

        if (result) *result = 0;

        eventFiltered = true;
    }

    return eventFiltered;
}


/**
 * @brief Called when we receive a DDE command
 * @param command
 * @param params
 * @return
 */
void IngescapeApplicationPrivate::onDdeCommand(const QString &command, const QString &params)
{
    QRegExp regexCommand("^\"(.*)\"$");
    bool singleCommand = regexCommand.exactMatch(params);
    if (
        singleCommand
        &&
        (command.compare("open", Qt::CaseInsensitive) == 0)
        )
    {
        if (_parent != nullptr)
        {
            QString filePath = regexCommand.cap(1);
            _parent->addPendingOpenFileRequest(QUrl::fromLocalFile(filePath), filePath);
        }
    }
    // Else: unsupported command
}



/**
 * @brief Sets specified value in the registry under HKCU\Software\Classes, which is mapped to HKCR then.
 */
bool IngescapeApplicationPrivate::SetHkcrUserRegKey(QString key, const QString &value, const QString &valueName)
{
    bool succeeded = false;

    qInfo() << "Add key in HKEY_CURRENT_USER";
    qInfo() << "- Register=" << "Software\\Classes\\" + key;
    qInfo() << "- ValueName=" << valueName;
    qInfo() << "- Value=" << value;
    qInfo() << "";

    HKEY hKey;
    key.prepend("Software\\Classes\\");

#ifdef UNICODE
    auto szKey = reinterpret_cast<const WCHAR *>(key.utf16());
#else
    QByteArray sKey = key.toLocal8Bit();
    auto szKey = sKey.constData();
#endif

    LONG lRetVal = RegCreateKey(HKEY_CURRENT_USER, szKey, &hKey);
    if (ERROR_SUCCESS == lRetVal)
    {
#ifdef UNICODE
        auto szValue = reinterpret_cast<const WCHAR *>(valueName.utf16());
#else
        QByteArray sValue = valueName.toLocal8Bit();
        auto szValue = sValue.constData();
#endif

        LONG lResult = ::RegSetValueEx(hKey, szValue[0] ? nullptr : szValue, 0, REG_SZ, (CONST BYTE *) value.utf16(), (value.length() + 1) * sizeof(quint16));

        if ((::RegCloseKey(hKey) == ERROR_SUCCESS) && (lResult == ERROR_SUCCESS))
        {
            succeeded = true;
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "warning: failed to set key" << key;
        }
    }
    else
    {
        TCHAR buffer[4096];
        auto size = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, lRetVal, 0, buffer, 4096, 0);
#ifdef UNICODE
        auto errorMessage = QString::fromWCharArray(buffer, size);
#else
        auto errorMessage = QString::fromLocal8Bit(buffer, size);
#endif

        qWarning() << Q_FUNC_INFO << "warning: failed to set key" << key
                   << ", Error=" << errorMessage;
    }

    return succeeded;
}


/**
 * @brief Register a file
 * @param documentId
 * @param fileTypeName
 * @param fileExtension
 * @param appIconIndex
 */
void IngescapeApplicationPrivate::registerFileType(const QString &documentId, const QString &fileTypeName, const QString &fileExtension, qint32 appIconIndex)
{
    // first register the type ID of our server
    if (!SetHkcrUserRegKey(documentId, fileTypeName))
    {
        qWarning() << Q_FUNC_INFO << "warning: failed to register file type"
                   << fileTypeName;
        return;
    }

    if (!SetHkcrUserRegKey(QString("%1\\DefaultIcon").arg(documentId),
            QString("\"%1\",%2")
                .arg(QDir::toNativeSeparators(
                    QApplication::applicationFilePath()))
                .arg(appIconIndex)))
    {
        qWarning() << Q_FUNC_INFO << "warning: failed to register default icon of documentId"
                   << documentId;
        return;
    }


    // Register DDE commands
    registerCommand("Open", documentId, " %1", "[open(\"%1\")]");

#ifdef UNICODE
    auto szExtension = reinterpret_cast<const WCHAR *>(fileExtension.utf16());
#else
    QByteArray sExtension = fileExtension.toLocal8Bit();
    auto szExtension = sExtension.constData();
#endif
    LONG lSize = _MAX_PATH * 2;
    TCHAR szTempBuffer[_MAX_PATH * 2];
    LONG lResult =
        ::RegQueryValue(HKEY_CLASSES_ROOT, szExtension, szTempBuffer, &lSize);

#ifdef UNICODE
    QString temp = QString::fromWCharArray(szTempBuffer, lSize);
#else
    QString temp = QString::fromLocal8Bit(szTempBuffer, lSize);
#endif

    if (
        (lResult != ERROR_SUCCESS)
        ||
        temp.isEmpty()
        ||
        (temp == documentId)
        )
    {
        // no association for that suffix
        if (!SetHkcrUserRegKey(fileExtension, documentId))
        {
            qWarning() << Q_FUNC_INFO << "warning: failed to associated file extension" << fileExtension
                       << "with documentId=" << documentId;
            return;
        }

        SetHkcrUserRegKey(QString("%1\\ShellNew").arg(fileExtension), QString(), "NullFile");
    }
}


/**
 * @brief Register a DDE command (e.g. oepen)
 * @param command
 * @param documentId
 * @param cmdLineArg
 * @param ddeCommand
 */
void IngescapeApplicationPrivate::registerCommand(const QString &command, const QString &documentId, const QString cmdLineArg, const QString ddeCommand)
{
    QString commandLine = QDir::toNativeSeparators(QApplication::applicationFilePath());
    commandLine.prepend(QLatin1Char('"'));
    commandLine.append(QLatin1Char('"'));

    if (!cmdLineArg.isEmpty())
    {
        commandLine.append(QLatin1Char(' '));
        commandLine.append(cmdLineArg);
    }

    if (!SetHkcrUserRegKey(QString("%1\\shell\\%2\\command").arg(documentId).arg(command),
            commandLine))
        return; // just skip it

    if (!ddeCommand.isEmpty())
    {
        if (!SetHkcrUserRegKey(
                QString("%1\\shell\\%2\\ddeexec").arg(documentId).arg(command),
                ddeCommand))
            return;

        if (!SetHkcrUserRegKey(QString("%1\\shell\\%2\\ddeexec\\application")
                                      .arg(documentId)
                                      .arg(command),
                appAtomName))
            return;

        if (!SetHkcrUserRegKey(QString("%1\\shell\\%2\\ddeexec\\topic")
                                      .arg(documentId)
                                      .arg(command),
                systemTopicAtomName))
            return;
    }
}



#endif





//---------------------------------------------------------------------
//
//
//  IngescapeApplication
//
//
//---------------------------------------------------------------------


/**
 * @brief Default constructor
 * @param argc
 * @param argv
 */
IngescapeApplication::IngescapeApplication(int &argc, char **argv)
    : QApplication(argc, argv),
      _hasPendingOpenFileRequest(false),
      _currentWindow(nullptr),
      _pendingOpenFileRequestUrl(""),
      _pendingOpenFileRequestFilePath(""),
      _privateAPI(new IngescapeApplicationPrivate(this))
{
#ifdef Q_OS_WIN
    installNativeEventFilter(_privateAPI.data());
#endif
}


/**
 * @brief Destructor
 */
IngescapeApplication::~IngescapeApplication()
{
    // Clean-up our window
    setcurrentWindow(nullptr);
}


/**
* @brief get our current window
* @return
*/
QQuickWindow* IngescapeApplication::currentWindow() const
{
   return _currentWindow;
}


/**
 * @brief Set our current window
 * @param value
 * @return
 */
bool IngescapeApplication::setcurrentWindow (QQuickWindow* value)
{
    bool hasChanged = false;

    if (_currentWindow != value)
    {
        if (_currentWindow != nullptr)
        {
            disconnect(_currentWindow, &QQuickWindow::destroyed, this, nullptr);
            _unsubscribeToCurrentWindow(_currentWindow);
        }

        _currentWindow = value;

        if (_currentWindow != nullptr)
        {
            _subscribeToCurrentWindow(_currentWindow);
            connect(_currentWindow,  &QQuickWindow::destroyed, this, &IngescapeApplication::_oncurrentWindowDestroyed);
        }

        hasChanged = true;

        Q_EMIT currentWindowChanged(value);
    }

    return hasChanged;
}


/**
 * @brief Returns a pointer to the application's casted as an IngescapeApplication
 * @return
 */
IngescapeApplication* IngescapeApplication::instance()
{
    return dynamic_cast<IngescapeApplication*>(QApplication::instance());
}


/**
 * @brief Override QApplication::event
 * @param event
 * @return
 */
bool IngescapeApplication::event(QEvent *event)
{
    if (
        (event != nullptr)
        &&
        (event->type() == QEvent::FileOpen)
        )
    {
        QFileOpenEvent *fileOpenEvent = dynamic_cast<QFileOpenEvent*>(event);
        if (fileOpenEvent != nullptr)
        {
            addPendingOpenFileRequest(fileOpenEvent->url(), fileOpenEvent->file());
        }
        // Else: should not happen because (type() == QEvent::FileOpen) means that we have a QFileOpenEvent event

        return true;
    }

    return QApplication::event(event);
}


/**
 * @brief Get our pending "open file" request
 * @return
 */
QPair<QUrl, QString> IngescapeApplication::getPendingOpenFileRequest()
{
    QPair<QUrl, QString> result = QPair<QUrl, QString>(_pendingOpenFileRequestUrl, _pendingOpenFileRequestFilePath);

    // Clean-up request
    _pendingOpenFileRequestUrl = QUrl();
    _pendingOpenFileRequestFilePath = "";
    sethasPendingOpenFileRequest(false);

    return result;
}


/**
 * @brief Add a pending "open file" request
 *
 * @param fileUrl
 * @param filePath
 */
void IngescapeApplication::addPendingOpenFileRequest(QUrl fileUrl, QString filePath)
{
    // Save this new request
    _pendingOpenFileRequestUrl = fileUrl;
    _pendingOpenFileRequestFilePath = filePath;

    // Notify request
    sethasPendingOpenFileRequest(true);
    Q_EMIT openFileRequest(_pendingOpenFileRequestUrl, _pendingOpenFileRequestFilePath);
}



/**
 * @brief Register a file type
 *
 * @param documentId    e.g. ApplicationName.myextension
 * @param fileTypeName  e.g. My file type
 * @param fileExtension e.g. .myextension
 * @param appIconIndex
 */
void IngescapeApplication::registerFileType(const QString &documentId, const QString &fileTypeName, const QString &fileExtension, qint32 appIconIndex)
{
#ifdef Q_OS_WIN
    //
    // Windows
    //
    _privateAPI->registerFileType(documentId, fileTypeName, fileExtension, appIconIndex);

#else
    //
    // Non Windows
    //
    Q_UNUSED(documentId)
    Q_UNUSED(fileTypeName)
    Q_UNUSED(fileExtension)
    Q_UNUSED(appIconIndex)

#endif
}


/**
* @brief Called when our current window is destroyed
*/
void IngescapeApplication::_oncurrentWindowDestroyed(QObject*)
{
   _currentWindow = nullptr;
   Q_EMIT currentWindowChanged(nullptr);
}


/**
 * @brief Subscribe to our current window
 */
void IngescapeApplication::_subscribeToCurrentWindow(QQuickWindow* window)
{
    if (window != nullptr)
    {
#ifdef Q_OS_WIN
        _privateAPI->enableDdeCommands();
#endif
    }
}


/**
 * @brief Unsubscribe to our current Window
 */
void IngescapeApplication::_unsubscribeToCurrentWindow(QQuickWindow* window)
{
    if (window != nullptr)
    {
#ifdef Q_OS_WIN
        _privateAPI->disableDdeCommands();
#endif
    }
}


