
#include "engine.h"
/* Prediction score: one point in round one, doubling each subsequent round.
   The browser renders the ordered table using eb_score_player(). */
int eb_leaderboard_rank(int player){if(player<0||player>=eb_player_count())return 0;int rank=1;for(int i=0;i<eb_player_count();i++)if(eb_score_player(i)>eb_score_player(player))rank++;return rank;}
