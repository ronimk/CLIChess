# CLIChess
![A school project for Christmas break 2019](CLIChess.png)
<br>
<br>
Tested with Microsoft Visual Studio Community 2019
<br>
<hr>
TODO:
<br>
<ul>
  <li>Update and upload the project documentation</li>
  <li>If three or more pieces of the same type are able to move to the same square and they are all in a symmetrical position from the target system, one source rank specifier may not be enough to discern which piece should move. In those cases the parser, as it is now, fails. Fix that.</li>
  <li>Abstract the square validity checking inside the Square class itself. This also means having to make changes to the way the parser handles source square specifications.</li>
</ul>
<hr>
<br>
Due to the console-specific printing routines, the game is not portable to non-Windows platforms. 
(these routines are in the CLIChess.cpp clearScreen function and in the GameManager.cpp
printBoard-method)
<br>
<br>
Supports the following commands (typable either into the "[CLIChess] >" or the "... to move:" prompt):<br>
n - starts a new game.<br>
q - quits the program.<br>
l filename - loads the file with the given filename.<br>
m - prints the main menu
<br>
<br>
Three additional commands can be given during a chess game: (typable only into "... to move:" prompt):<br>
s filename - saves the current game into a file with the given file name.<br>
t # - takes back a user specified amount of moves; for example "t 5" takes back 5 moves. <br>
b - prints the gameboard.
<br>
<br>
During the game, the user inputs moves in the algebraic chess notation, EXCEPT:<br>
The user does not have to input check, checkmate, promotion or En Passant moves.
<br>
<br>
<hr>
Known issues:
<hr>
1. If the user wants to type special notation (checks, En Passants, etc.), the game only
recognizes promotion moves and discards the rest of the special notations.
<br>
<br>
2. Any garbage can be inputted after a special move notation: the program happily parses
them out of the input string.
<br>
<br>
3. Due to the multi-phased nature that the move validation process is made (first initial parsing, then
intermediate information extraction, then the actual move validation and finally move committing, if
all has gone well so far - abandoning the entire move validation process at the first sight of an error
with no further processing performed afterwards), there are probably certain error messages that seem
weird or counter-intuitive to the users.
<br>
<br>
4. No stalemate after three identical board states implemented. However, all the other chess rules are implemented.
<br>
<br>
5. The user can type a move like 1. Nbc3, even when the given extra source square information in the move is unnecessary.
Removing this issue requires appending the MoveResultsAnalysis a little bit and making an extra check after the initial collecting
of all the possible pieces that can make the given move to check whether there are exactly one, or more than one piece that
can make the given move. I felt this to be troublesome and unnecessary for the purpouses of the program and therefore I did not
implement such an error check. 
<br>
<br>
<hr>
A note about the source code:
<hr>
The original project requirements did not include any special moves (En Passants, Castling or Promotions).
Therefore my first idea was to focus mainly on the parsing and analysis of the moves given in algebraic chess
notation. Once I had gotten all the basic moves done, I felt that I wanted to try more: I wanted to make a real
chess program usable for any chess player to practice on (that's why there exists the possibility to take back
as many moves as the user likes, for example).
<br>
<br>
This, however, made the handling of the special moves a bit messy. For example, the castling and the promotion
had to be handled separately and in a very different manner from the analysis of normal moves, which complicated
the GameManager quite a lot. On the other had, since the move rules of chess are very heterogenous, they
will complicate a program design anyway.
<br>
<br>
The MoveAnalysisResults-ds with a purely public interface was designed for easy passing of any relevant
information between the parser, the move validator and the actual move maker. It (and some other parts of
the program) may not exactly follow the best OO practices, but it's a very convenient ds and performs its duty
well.
<br>
<br>
There are still two or three "problem areas" in the code that are desperately in need of refactoring (like changing
the parser into a state machine instead of the current "ad hoc" solution, but overall, I'm very satisfied with the
results.
