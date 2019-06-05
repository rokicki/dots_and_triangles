Dots and Triangles is a variation of the classic game
Dots and Boxes, except instead of making boxes on a square grid
you make triangles on a triangular grid.  Other than this the
game and its strategy remains much the same.

Commonly dismissed as a children's game, Dots and Boxes (and
this variant) are actually significant and complex games, worthy
of study.  One of the country's leading mathematicians,
Elwyn Berlekamp (who sadly died just two months ago) wrote a
deep and popular book on the game, and the game is often played
at some mathematics conferences between top mathemticians.

Before trying this as a
programming challenge, try playing it against your neighbor,
or perhaps just try to solve the following puzzle.  What's the
appropriate strategy to win from this position?  (All horizontal
edges are chosen; no slanted edges are chosen; it's your move.)

          1
         . .
        2---3
       . . . .
      4---5---6
     . . . . . .
    7---8---9--10

Clearly you have to give up some triangles to your competitor.
There is one triangle on the top row, three on the next, and
five on the last, so you give up the top triangle, and your
competitor accepts it, and then moves in the smallest remaining
row of triangles, the second row.

          1
         /B\
        2---3
       / . . .
      4---5---6
     . . . . . .
    7---8---9--10

Now you can take five triangles, but then you have to give
seven back, losing the game:

          1
         /B\
        2---3
       /A\A/A\
      4---5---6
     / . . . . .
    7---8---9--10

Is there another way to proceed?

This simple strategy, called the "double cross" strategy, is
effective against almost all players.  But once you master the
double cross strategy, there are many other levels of depth and
strategy to this game (usually on larger boards), including some
surprising mathematics.  Berlekamp's book is well worth the read.

Now on to coding.  This is what's known as an impartial game,
meaning the same moves are available to both players.  This
allows the game to be represented as a "game tree" or "search
tree", where each node in the tree corresponds to a position
of the game and is labeled by the value of that position.
The value of a position is the best score reachable from
that position for the player about to move, assuming both
players play optimally and ignoring any score that has
accrued from previous moves.  For instance, in the last
position shown above, the value of the position is 5, since
the next player to move can simply take all five remaining
triangles.

What does it mean for a player to play optimally?  Intuitively,
this means they win if they can; they always choose a move
that allows them to win if there is such a move.  If both
players play optimally, each player plays to maximize their
own score, and minimize the score of the other player.  This
can be implemented as a recursive search routine that looks
roughly like this:

    int score(position board) {
       if (no_moves_possible())
          return 0 ; // no more triangles
       int r = NEG_INF ; // most negative number
       for (edge e in available_edges(board)) {
          position new_board = add_edge(board, e) ;
          int triangles_made = deltascore(board, e) ;
          if (triangles_made > 0) // still my move
             r = max(r, triangles_made + score(new_board)) ;
          else       // other player; negate score
             r = max(r, -score(new_board)) ;
       }
       return r ;
    }

This code assumes some abstract representation of the
board, and utility routines to update the board position with
a new edge and to calculate the number of new triangles
created by a particular move.

At each level, the "ply" of the search tree (the number
of children nodes that need to be explored) is the number
of available edges.  From an empty board, there are 18
available edges.  The next level down has 17 available edges,
and so on and so forth, so the total run time is proportional
to 18! from an empty board.  This is a pretty big number:
6,402,373,705,728,000, so we can't expect it to run in time
from an empty board.  But the problem statement says that at
least 6 moves will have been made already, so we only need
to evaluate the search tree from a board with 12 available
positions, and 12! is a much more reasonable 479,001,600.
This may still be too slow if the number of board positions
is high, but we will improve this later.

For many games the only thing we need calculate in the game
tree is a boolean value indicating if this position is a
win or not (the score doesn't matter).  But for this game,
in order to know if we win, we need to know not just how
many triangles we can make from this position if we play
optimally, but also how many triangles we and our competitor
have so far.  We can put the information on triangles made
so far into the abstract position data structure, but doing
significantly increases the number of possible states that
the board can be in.  Later we will exploit the small size
of the state space to make our program faster, and not
having the labels of who owns what triangle, or even the
counts of how many triangles are owned by each player, will
allow us to make the code faster.

How can we easily represent the board?  If we ignore the
labels of already created triangles, the only information is
whether a particular edge has been marked or not.  We could
use a boolean array for this, but since the count of edges
is fixed and small, we can instead just use a single integer,
with individual bits representing the state of a single
edge.  To check bit b in an integer i, you use a statement
like

    if (0 == ((i >> b) & 1)) ...

To set bit b in an integer i, you do this:

    i |= 1 << b

Implementing the remaining bits of the solution is annoying
but not too bad.  We need a way to identify each of the
edges; we need to know which edges are in each triangle; we
need to read and evaluate the moves already made.

Perhaps the simplest (though not most general) way to relate
vertices to edges and edges to triangles is to just draw
the board on a sheet of paper, number the vertices as given
in the problem statement, number the edges arbitrarily, and
then make an explicit literal array that gives the vertex
pairs for each edge and the edge identifiers for each
triangle.  The actual numbering is irrelevant.  In the example
Java code, for edge numbering, I create a literal array of
triples, where each triple has two vertex numbers and an
edge number.  I use a simple one-dimensional array and
group them by convention, rather than make an explicit
two-dimensional array or use a triple data structure, just
for brevity.  I also write a similar array that gives a
triple for each triangle, where the components of the triple
are the edge numbers for that triangle.


