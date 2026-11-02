
#include "engine.h"
/* Bracket resolution is intentionally owned by the C engine.
   Match participants are derived from the previous round's recorded winners. */
int eb_bracket_is_valid(void){
 if(eb.status<2)return 0;
 for(int r=0;r<eb_rounds();r++)for(int m=0;m<eb_match_count(r);m++){
   int a=eb_match_team(r,m,0),b=eb_match_team(r,m,1),w=eb_match_winner(r,m);
   if(w>=0&&(w!=a&&w!=b))return 0;
 }
 return 1;
}
