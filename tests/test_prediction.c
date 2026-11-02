
#include "../src/engine.h"
#include <assert.h>
#include <stdio.h>
int main(void){eb_reset();assert(eb_create("Prediction","Game",4));for(int i=0;i<4;i++){char n[10];sprintf(n,"Team%d",i);assert(eb_register_team(-1,n,"","#abcdef"));}assert(eb_start());int a=eb_match_team(0,0,0),b=eb_match_team(0,0,1),c=eb_match_team(0,1,0),d=eb_match_team(0,1,1);
assert(eb_predict(0,0,0,a));assert(eb_predict(0,0,1,c));
assert(eb_pick_team(0,1,0,0)==a&&eb_pick_team(0,1,0,1)==c);
assert(eb_predict(0,1,0,a));assert(eb_player_pick(0,1,0)==a);assert(eb_score_player(0)==0);assert(eb_run_prediction());assert(eb_prediction_winner(1,0)>=0);assert(eb_run_upset(123));assert(eb_upset_winner(1,0)>=0);assert(!eb_predict(0,0,0,15));puts("test_prediction: PASS");}
