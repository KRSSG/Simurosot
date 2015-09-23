Simurosot : 
SimuroSot is an event in FIRA which consists of a server which has the soccer game environments and 
two client programs with the game strategies .

Platform : Windows (Visual Studios)

User Manual : 
1. Download Simulator and Strategy folder from the FIRA website (http://www.fira.net/contents/sub03/sub03_7.asp) and move it to C drive .
1. Clone the repository to your directory .
# If running the project for first time 
  i.  Under cloned folder , open ConsoleDebugger and then open ConsoleDebugger.sln file (Visual Studios sln) 
  ii. Build -> Build Solution 

2. In cloned repository , open "Run.bat" in text editor 
  i.  Edit line 4 : 
       COPY C:\"YOUR FOLDER"\Simurosot\Release\Strategy.dll C:\Strategy\blue
  ii. Make sure all paths corresponds to their correct file 
  
3. In cloned repository , open Simurosot and then open Strategy.sln file (Visual Studios sln)
4. Code the strategy , Build it and open "Run.bat" by double-clicking it 
