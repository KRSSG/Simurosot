TASKKILL /F /IM RobotSoccer.exe
TASKKILL /F /IM ConsoleDebugger.exe
DEL C:\Strategy\blue\Team1.dll
<<<<<<< HEAD
COPY C:\simurosot_2015\SimuroSot\Release\Strategy.dll C:\Strategy\blue
=======
COPY .\SimuroSot\Debug\Strategy.dll C:\Strategy\blue
>>>>>>> 80d2211d2483d8f020f6d3782692107a47a84463
REN C:\Strategy\blue\Strategy.dll Team1.dll
START .\ConsoleDebugger\Debug\ConsoleDebugger.exe
START C:\Debugger_64bit\RobotSoccer.exe


