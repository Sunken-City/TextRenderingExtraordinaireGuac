 ___  _                           _  _  _        
| . \<_> ___  ___  ___  _ _  ___ | |<_>| |__ ___ 
|  _/| |/ | '/ . \/ . || | |/ ._>| || || / // ._>
|_|  |_|\_|_.\___/\_. |`___|\___.|_||_||_\_\\___.
                  <___'                          

Controls:
H - Move Left
J - Move Down
K - Move Up
L - Move Right
Y - Move Up Left
U - Move Up Right
B - Move Down Left
N - Move Down Right
M - Hold to display entire map
P - Make a pathfinding step
Shift + P - Run performance testing to the console.

Known Issues:
1. There is currently a huge framerate problem and memory leak currently in the build, due to work in DFS. 
The game will crash after a few minutes because it leaks so much memory. This is tied to the draw calls, so please render the full map only as needed.
The framerate issues (once again tied to the bugs left from converting to modern rendering in DFS) may cause you to drop inputs, 
so please hold down the keys you want until they perform the action you'd like them to.

2. A* may not always pick the most efficient path, but it's doing a pretty decent job, so I decided to run with it for the time being 
and resolve any remaining bugs after talking with you in class.