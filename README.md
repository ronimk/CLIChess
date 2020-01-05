# CLIChess
![A school project for Christmas break 2019](CLIChess.png)
<br>
<br>
Tested with Microsoft Visual Studio Community 2019
<br>
<br>
Due to the console-specific printing routines, not portable to non-Windows platforms.<br>
(these routines are in the CLIChess.cpp ClearScreen function and the GameManager.cpp'<br>
printBoard-method)
<br>
<br>
Supports the following commands (typed into the "[CLIChess] >" prompt):<br>
n - starts a new game.<br>
q - quits the program.<br>
l filename - loads the file with the given filename.<br>
<br>
<br>
Two additional commands can be given during a chess game:<br>
s filename - saves the current game into a file with the given file name.<br>
t # - takes back a user specified amount of moves; for example "t 5" takes back 5 moves.
<br>
<br>
During the game, the user inputs moves in the algebraic chess notation, EXCEPT:<br>
The user does not have to input check, checkmate, promotion or En Passant moves.
<br>
<br>
Known issues:
<br>
<br>
1. If the user wants to type special notation (checks, En Passants, etc.), the game only<br>
   recognizes promotion moves and discards the rest of the special notations.
<br>
<br>
2. Any garbage can be inputted after a special move notation: the program happily parses<br>
   them out of the input string.
<br>
<br>
3. Due to the multi-phased nature that the move checks are made (first initial parsing, then<br>
   intermediate information extraction, then move validation and finally move committing, if<br>
   all has gone well so far - with abandoning the move validation process at the first sigh<br>
   of an error and no further processing performed afterwards), there are probably certain error<br>
   messages that seem weird or counter-intuitive to the users.