#include <iostream>
#include <map>
#include <vector>
#include <assert.h>
typedef unsigned int bits ;
using namespace std ;
map<pair<int, int>, int> vertex_id ; // coordinate pair to vertex number
map<pair<int, int>, int> edge_id ; // vertex pair to edge number
vector<pair<int, int>> edges ; // list of edges by id
int edgecount ;
const int BOARD_HEIGHT = 4 ;
// the directions of the edges; down, diagonal, right
int dx[] = { 0, 1, 1 } ;
int dy[] = { 1, 1, 0 } ;
vector<bits> triangles ;
// is this x,y pair in the triangle?
bool intri(int x, int y) {
   return x >= 0 && y >= 0 && x < BOARD_HEIGHT && y < BOARD_HEIGHT && x <= y ;
}
// what is the bit number for this triangle
bits edgebit(int x, int y) {
   return 1LL<<edge_id[{x,y}] ;
}
// number the vertices, edges, and find the triangles
void setup_board() {
   // we skew the top of the triangle to the left and make it the origin
   // y increases downward, x increases rightward
   // number vertices from 1; these must match input diagram
   int vertnum = 1 ;
   for (int y=0; y<BOARD_HEIGHT; y++)
      for (int x=0; x<=y; x++)
         vertex_id[{x,y}] = vertnum++ ;
   // number edges from 0
   int edgenum = 0 ;
   for (int y=0; y<BOARD_HEIGHT; y++)
      for (int x=0; x<=y; x++)
         for (int d=0; d<3; d++) {
            int v1_id = vertex_id[{x,y}] ;
            int x2 = x + dx[d] ;
            int y2 = y + dy[d] ;
            if (intri(x2, y2)) {
               int v2_id = vertex_id[{x2,y2}] ;
               edges.push_back({v1_id,v2_id}) ;
               edge_id[{v1_id,v2_id}] = edgenum ;
               edge_id[{v2_id,v1_id}] = edgenum ;
               edgenum++ ;
            }
         }
   edgecount = edgenum ;
   // find triangles, as bit masks.  Each point can either be the top or
   // bottom of a triangle.
   for (int y=0; y<BOARD_HEIGHT; y++)
      for (int x=0; x<=y; x++) {
         // upward triangle
         int v1 = vertex_id[{x,y}] ;
         if (intri(x-1, y-1) && intri(x, y-1)) {
            int v2 = vertex_id[{x-1,y-1}] ;
            int v3 = vertex_id[{x,y-1}] ;
            triangles.push_back(edgebit(v1,v2)|edgebit(v2,v3)|edgebit(v1,v3)) ;
         }
         // downward triangle
         if (intri(x+1, y+1) && intri(x, y+1)) {
            int v2 = vertex_id[{x+1,y+1}] ;
            int v3 = vertex_id[{x,y+1}] ;
            triangles.push_back(edgebit(v1,v2)|edgebit(v2,v3)|edgebit(v1,v3)) ;
         }
      }
   assert(edgenum <= (int)(sizeof(bits) * 8)) ; // make sure we have enough bits
}
int deltascore(bits b1, bits b2) {
   int r = 0 ;
   for (int i=0; i<(int)triangles.size(); i++) {
      bits m = triangles[i] ;
      if ((b1 & m) != m && (b2 & m) == m)
         r++ ;
   }
   return r ;
}
vector<int> forcedmoves ;
// minimax of impartial game; negate when we switch players
const int SENTINEL = -1000 ; // indicates no cache entry or no legal moves
vector<int> cache ;
int recur(int movenum, bits board) {
   if (cache[board] > SENTINEL)
      return cache[board] ;
   int r = SENTINEL ;
   for (int e=0; e<edgecount; e++) {
      if ((board >> e) & 1)
         continue ; // edge is used
      if (movenum < (int)forcedmoves.size() && e != forcedmoves[movenum])
         continue ;
      bits nboard = board | (1LL << e) ;
      int s = deltascore(board, nboard) ;
      int t ;
      if (s > 0) // same player moves
         t = s + recur(movenum+1, nboard) ;
      else // other player moves
         t = - recur(movenum+1, nboard) ;
      if (movenum == (int)forcedmoves.size())
         cout << "First move " << edges[e].first << "," << edges[e].second << " gives " << t << endl ;
      r = max(t, r) ;
   }
   if (r == SENTINEL) // no legal moves; value of 0
      r = 0 ;
   cache[board] = r ;
   return r ;
}
int main() {
   setup_board() ;
   cache.resize(1LL << edgecount) ;
   int T ;
   assert(cin >> T) ;
   for (int game=1; game<=T; game++) {
      for (int i=0; i<(int)cache.size(); i++)
         cache[i] = -1000 ;
      int nmoves = 0 ;
      assert(cin >> nmoves) ;
      bits usedmoves = 0 ;
      forcedmoves.clear() ;
      for (int i=0; i<nmoves; i++) {
         int v1, v2 ;
         assert(cin >> v1 >> v2) ;
         assert(edge_id.find({v1,v2}) != edge_id.end()) ;
         int e = edge_id[{v1,v2}] ;
         assert(!((usedmoves >> e) & 1)) ; // make sure this move isn't a dup
         forcedmoves.push_back(e) ;
         usedmoves |= 1LL << e ;
      }
      int sc = recur(0, 0) ;
      cout << "Game " << game << ": " ;
      if (sc < 0) {
         cout << "B wins." << endl ;
      } else if (sc > 0) {
         cout << "A wins." << endl ;
      } else {
         cout << "Tie game." << endl ;
      }
   }
}
