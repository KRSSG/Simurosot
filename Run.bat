TASKKILL /F /IM RobotSoccer.exe
TASKKILL /F /IM ConsoleDebugger.exe
DEL C:\Strategy\blue\Team1.dll
COPY C:\simurosot_2015\SimuroSot\Debug\Strategy.dll C:\Strategy\blue
REN C:\Strategy\blue\Strategy.dll Team1.dll
START .\ConsoleDebugger\Debug\ConsoleDebugger.exe
START C:\CodeOSoccer_64bit\Debugger_64bit\RobotSoccer.exe


