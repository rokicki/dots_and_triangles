Dots and Triangles is a variation of the classic game
Dots and Boxes, except instead of making boxes on a square grid
you make triangles on a triangular grid.  Other than this the
game and its strategy remains much the same.

Commonly dismissed as a children's game, Dots and Boxes (and
this variant) are actually significant and complex games, worthy
of study.  One of the country's leading mathematicians,
Elwyn Berlekamp (who sadly died just two months ago) wrote a
deep and popular book on the game.  Before trying this as a
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
