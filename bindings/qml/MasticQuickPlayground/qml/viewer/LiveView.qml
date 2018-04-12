import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4


Item {
    id: root

    //-------------------------------------------
    //
    // Properties
    //
    //-------------------------------------------


    // Font
    property font font: Qt.font({family: "Helvetica", pixelSize: 12});



    // URL of the QML file to render
    property url source

    // Source code
    property string sourceCode


    // Component model that we try to load
    property var _componentModel: null

    // Innstance of our component model
    property var _componentInstance: null




    //-------------------------------------------
    //
    // Sginals
    //
    //-------------------------------------------

    // Triggered when our content is loaded
    signal contentLoaded();


    // Triggered when we have failed to load a content
    signal contentError();


    //-------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------

    // Throttle function used to avoid useless updates
    function throttle(timer, delay, callback) {
        var now = new Date().getTime();

        if (
                (timer.lastTime && (now < (timer.lastTime + delay)))
                ||
                !timer.running
                )
        {
            // Reset timer
            timer.stop();
            timer.callback = callback;
            timer.interval = delay;
            timer.lastTime = now;

            // Start timer
            timer.start();
        }
        else
        {
            timer.lastTime = now;
        }
    }


    // Clear our content
    function clearContent()
    {
        if (_componentInstance !== null)
        {
            _componentInstance.destroy()
            _componentInstance = null;
        }
        _componentModel = null;
    }


    // Load our component from a file
    function loadComponentFromFile()
    {
        // Clean-up
        clearContent();

        // Load content after a short delay TO BE SURE that our previous content is destroyed
        throttle(timerLoadContent, 10, function() {
            _loadComponentFromFile();
        });
    }


    // Load our component from a file
    function _loadComponentFromFile()
    {
        // Check if we need to load something
        if (source !== "")
        {
            _componentModel = Qt.createComponent(source)
            if (_componentModel !== null)
            {
                // Check if we need more time to load our component
                if (
                    (_componentModel.status === Component.Ready)
                    ||
                    (_componentModel.status === Component.Error)
                   )
                {
                    componentLoadedFromFile()
                }
                else
                {
                    _componentModel.statusChanged.connect(_componentLoadedFromFile)
                }
            }
        }
    }


    // Called when our component is loaded from a file
    function _componentLoadedFromFile()
    {
        if (_componentModel !== null)
        {
            if (_componentModel.status === Component.Ready)
            {
                errorMessage.text = ""
                _componentInstance = _componentModel.createObject(content, {});

                root.contentLoaded();
            }
            else if (_componentModel.status === Component.Error)
            {
                var sourceFilePath = source.toString()
                var error = _componentModel.errorString().replace(new RegExp(sourceFilePath + ':(\\d+)', 'g'), "Line $1: ")
                errorMessage.text = error

                root.contentError();
            }
        }
        else
        {
            errorMessage.text = ""

            root.contentLoaded();
        }
    }



    // Load our component from a string
    function loadComponentFromString()
    {
        // Clean-up
       clearContent();

        // Load content after a short delay TO BE SURE that our previous content is destroyed
        throttle(timerLoadContent, 10, function() {
            _loadComponentFromString();
        });
    }


    // Load our component from a string
    function _loadComponentFromString()
    {
        try
        {
            _componentInstance = Qt.createQmlObject(sourceCode, content);

            root.contentLoaded();
        }
        catch(exception)
        {
            var error = "";
            for(var index = 0; index < exception.qmlErrors.length; index++)
            {
                var qmlError = exception.qmlErrors[index];

                error += "Line " + qmlError.lineNumber + " : " + qmlError.message + "\n";
            }

            errorMessage.text = error;

            root.contentError();
        }
    }



    //-------------------------------------------
    //
    // Behavior
    //
    //-------------------------------------------

    onSourceChanged: {
        errorMessage.text = ""

        loadComponentFromFile()
    }


    onSourceCodeChanged: {
        errorMessage.text = ""

        loadComponentFromString()
    }


    // Timer used to load content
    Timer {
        id: timerLoadContent

        repeat: false
        running: false

        property var callback
        property var lastTime

        onTriggered: callback();
    }


    //-------------------------------------------
    //
    // Content
    //
    //-------------------------------------------

    // Error message
    Text {
        id: errorMessage

        anchors {
            fill: parent
            margins: 10
        }

        visible: (text.length !== 0)

        color: "red"

        text: ""

        wrapMode: Text.WordWrap

        font: root.font
    }


    // Content of our viewer
    Item {
        id: content

        anchors.fill: parent

        visible: !(errorMessage.visible)
    }
}
