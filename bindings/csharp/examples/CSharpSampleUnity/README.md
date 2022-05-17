Minimum Unity version to use : 2018.1

To open the project, launch Unity HUB. Click on the button "Add" and choose the project folder. Click on the project in HUB to open it in Unity.
Once Unity opened, go to Asset -> Scenes and double click on MainScene.unity to load the scene.

Make sure the ingescape library is installed on your system.
Don't forget to manually copy "IngescapeCSharp.dll" and "Ingescape.dll" into the "Plugins" folder.

To launch the project in Unity, click on the "play button" situated at the top-middle.

You can show all your unity logs through your ingescape environment by adding this code in the main class:

void Start()
{
    Application.logMessageReceived += LogMessageReceived;
    ...
    ...
}

private void LogMessageReceived(string condition, string stackTrace, LogType type)
{
    string[] stackTraceLines = stackTrace.Split('\n');
    string caller = (stackTraceLines.Length >= 2) ? stackTraceLines[1] : "";
    switch (type)
    {
        case (LogType.Assert):
        case (LogType.Error):
        case (LogType.Exception):
            Igs.Error(condition, caller);
            break;
        case (LogType.Warning):
            Igs.Warn(condition, caller);
            break;
        default:
            Igs.Info(condition, caller);
            break;
    }
}

private void OnApplicationQuit()
{
    Application.logMessageReceived -= LogMessageReceived;
    ...
    ...
}

