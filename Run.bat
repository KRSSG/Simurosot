TASKKILL /F /IM RobotSoccer.exe
TASKKILL /F /IM ConsoleDebugger.exe
DEL C:\Strategy\blue\Team1.dll
COPY .\SimuroSot\Debug\Strategy.dll C:\Strategy\blue
REN C:\Strategy\blue\Strategy.dll Team1.dll
START .\ConsoleDebugger\Debug\ConsoleDebugger.exe
START C:\Debugger_64bit\RobotSoccer.exe


