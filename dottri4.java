import java.util.* ;
public class dottri4 {
   final static int TRIANGLECOUNT = 9 ; // 9 total triangles
   final static int EDGECOUNT = 18 ;    // 18 total edges
   final static int SENTINEL = -1000 ;  // no legal moves
   static int edges[] = {        // triples are vertex, vertex, edge
      1,2,0, 1,3,1, 2,3,4, 2,4,2, 2,5,3, 3,5,5, 3,6,6, 4,5,9, 4,7,7, 4,8,8,
      5,6,12, 5,8,10, 5,9,11, 6,9,13, 6,10,14, 7,8,15, 8,9,16, 9,10,17
   } ;
   static int triangle_edges[] = {
      0,1,4, 2,3,9, 5,6,12, 7,8,15, 3,4,5, 10,11,16, 13,14,17, 8,9,10, 11,12,13
   } ;
   static int triangles[] = new int[TRIANGLECOUNT] ;
   static void setup_board() {
      for (int i=0; i<TRIANGLECOUNT*3; i++)
         triangles[i/3] |= 1 << triangle_edges[i] ;
   }
   static int deltascore(int b1, int b2) {
      int r = 0 ;
      for (int i=0; i<TRIANGLECOUNT; i++) {
         int m = triangles[i] ;
         if ((b1 & m) != m && (b2 & m) == m)
            r++ ;
      }
      return r ;
   }
   static int dp[] ;
   static void iterate() {
      for (int board=(1<<EDGECOUNT-1); board>=0; board--) {
         int r = SENTINEL ;
         for (int e=0; e<EDGECOUNT; e++) {
            if (0 != ((board >> e) & 1))
               continue ; // edge is used
            int nboard = board | (1 << e) ;
            int s = deltascore(board, nboard) ;
            int t ;
            if (s > 0) // same player moves
               t = s + dp[nboard] ;
            else // other player moves
               t = - dp[nboard] ;
            r = Math.max(t, r) ;
         }
         dp[board] = r ;
      }
   }
   public static void main(String[] args) {
      Scanner sc = new Scanner(System.in) ;
      setup_board() ;
      dp = new int[1<<EDGECOUNT] ;
      iterate() ;
      int T = sc.nextInt() ;
      int score = 0 ;
      char player = 'A' ;
      int board = 0 ;
      for (int game=1; game<=T; game++) {
         int nmoves = sc.nextInt() ;
         for (int i=0; i<nmoves; i++) {
            int v1=sc.nextInt(), v2=sc.nextInt() ;
            for (int j=0; j<edges.length; j += 3)
               if (v1 == edges[j] && v2 == edges[j+1] ||
                   v1 == edges[j+1] && v2 == edges[j]) {
                  int nboard = board | (1 << edges[j+2]) ;
                  int delta = deltascore(board, nboard) ;
                  if (delta > 0) {
                     score += delta ;
                  } else {
                     score = - score ;
                     player = (char)('A' + 'B' - player) ;
                  }
                  board = nboard ;
               }
         }
         if (score + dp[board] < 0)
            player = (char)('A' + 'B' - player) ;
         System.out.println("Game " + game + ": " + player + " wins.") ;
      }
   }
}
