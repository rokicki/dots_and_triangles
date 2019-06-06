#### Dots and Triangles

Dots and Triangles is a variation of the classic game
[Dots and Boxes](https://en.wikipedia.org/wiki/Dots_and_Boxes),
except instead of making boxes on a square grid
you make triangles on a triangular grid.  Other than this the
game and its strategy remains much the same.

Commonly dismissed as a children's game, Dots and Boxes (and
this variant) are actually significant and complex games, worthy
of study.  One of the country's leading mathematicians,
[Elwyn Berlekamp](https://en.wikipedia.org/wiki/Elwyn_Berlekamp)
(who sadly died just two months ago) wrote a
[deep and popular
book](https://www.amazon.com/Dots-Boxes-Game-Elwyn-Berlekamp/dp/1568811292)
on the game, and the game is often played
at some mathematics conferences between top mathematicians.

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

#### Solving the Dots and Triangles Problem

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

The result is dependent only on the arguments, and it contains
tests for a base case, and recursive calls; this is a
mathematical formulation, with no side effects.  More
concretely, this is a pure function.  This can make
reasoning about the program much easier.

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

#### Data Structures and Details

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

Using integers as bitfields to represent small boolean
vectors can dramatically speed up programs, often by
two orders of magnitude.

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

To execute the first moves of the game, we can add iterative
code that reads the vertex numbers, calculates the appropriate
edge number, sets the appropriate bits, and keeps track of the score
and current player.  But the actual game logic is already
embedded in our search routine, so there's no need to replicate
it.  Instead, we create a simple integer array called forced_moves
that holds the sequence of edges that begin the game.  We add
a parameter to our recursive search routine that says what move
number we are on.  And, in our search routine, we add a line
such as this to exclude non-matching moves at that level:

    if (movenum < forcedmoves.length &&
        currentedge != forcedmoves[movenum])
       continue ;

This trick can greatly simplify tree search where the first
part of the path down the tree is forced.

All of this goes together pretty easily; the sample Java code
is in the file dottri.java.  When we compile and run the code
on the sample data, we find it takes slightly over a minute
to run, which is too long.

#### Speeding up the Solution: Memoization

So far we've used brute force to evaluate the search tree.
But if we think about it, our search tree is not exactly a tree;
indeed, it is a DAG (directed acyclic graph) because there
are multiple ways to attain most board positions.  The edges
can be chosen in any particular order.  Our current code
re-evaluates identical board positions over and over, and this
makes it slow.  To fix this problem, all we need to do is
add a cache that remembers the result of evaluating a particular
board position, and if we encounter that same board position
again, we return the result from the cache rather than
reevaluating it.  This trick is called memoization (not a
typo).

Our board position is represented as an integer bit field with
at most 18 bits set.  If our edges are numbered compactly from
zero, then the maximum value this position integer can take is
2^18-1 or 262,143, a small number.  We create an integer array
of this size, and initialize it to some marker (sentinel) value
that is not a legal score (say, -1000).

Then, to our recursive routine, we add this code to the top of
the routine:

    if (cache[board] != SENTINEL)
       return cache[board] ;

and to the end of the routine we add this, right before returning
the result:

    cache[board] = r ;

With memoization it's important that the function being memoized
is a pure function, independent of side effects and dependent
only on its arguments.

We need to re-initialize the cache for each example, since the
forced moves change the search tree.  With this change our run
time decreases from e! (where there are e edges available in a
given position) to 2^e (here e is the number of edges on the
board); this is from half a billion to a quarter million, a huge
improvement.

The resulting code is in dottri2.java; executing it on our
sample input takes less than a fifth of a second, for a speedup
of about 500.

The problem statement does not say how many game positions there
will be, and our new code does not reuse the cache since it is
affected by those initial moves.  If there were many thousands of
game positions, this memoized version might be too slow.  This
can easily be fixed by not using the cache for levels of the tree
that are have forced moves.

#### Speeding up the Solution: Dynamic Programming

In the memoized recursive solution, we needed code to check the
cache and fill the caache.  Another approach based on ordering
the state space can speed the program up a lot, and solve other
problems that might occur with memoized recursion, such as
stack exhaustion.

Remember, we memoized a pure function.  The range of values of
the arguments of the function is called the state space.  If we
can make an assertion about the state space, such that the
recursive calls always explore a state that is less than
(for some definition of less than) the arguments of the outer
call, then we can order the state space by that predicate.
In this case, recursive calls always have more bits set (because
more edges are marked) than the outer call; indeed, the
recursive calls always have a greater state value than the
outer call.  Thus, by evaluating the nodes from the highest
state value to the least, we always know our recursive values
have already been calculated and are ready in the cache.

We can therefore replace the recursive call with an
iterative one, filling the cache with calculated values from
2^18-1 down to 0.  The code in dottri4.java shows this.
By convention dynamic programming solutions use an array
called dp to store the cached results.

For this particular problem, removing the recursion complicates
the code a tiny bit, because now we need to manage the
initial moves from the input file separately; there's no
easy way to integrate them into the iteration for states.

In many cases, with dynamic programming, you can actually
order the state space exploration and storage such that the
amount of memory required is significantly reduced.  That's
not trivially the case here, but for many state spaces that
are described by two ordinal values, you often need store only
a single row of the state space, rather than the entire
two-dimensional array.  With memoization it can be much
more difficult to determine when storage can be reclaimed.

#### Recursion vs. Memoization vs. Dynamic Programming

So when should you use plain recursion, or memoized recursion,
or dynamic programming?  Here are some features of each that
should be kept in mind.

* Plain old recursion can be very fast, especially if you
suspect that not much of the state space need be explored
to solve a particular problem.  Very frequently you should
start by writing the simplest recursive formulation of the
problem and just see if it works; often that will be all you
need to do.

* Memoized recursion works great if you know the depth of
the search tree is bounded (typical stack sizes permit
recursion thousands of levels deep but not millions of
levels deep).  Debugging very deep stack traces can be
painful.  Memoized recursion also explores only that part
of the state space required, where dynamic programming usually fills
the entire state space.  Memoized recursion has overhead in
checking the cache for valid values and in building and
destroying stack frames.  Memoized recursion works best if
the entire needed state space fits in memory; it is possible to
clear or use a bounded memoization cache if memory is tight, but
this can lead to exponential slowdown.

* Dynamic programming is great if you suspect you will need to
explore the entire state space, and if you can come up with a
reasonable way to order the state space so that subproblem
results will be available before the outer problem.  Dynamic
programming has no function call overhead, no cache checck
overhead, and can often be made extremely memory-efficient.
And it will never blow your stack.
