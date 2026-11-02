
#include "../src/engine.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 eb_reset();assert(eb_create("Test Cup","Valorant",4));
 assert(!eb_start());assert(!eb_register_team(-2,"Bad ID","","#11aaff"));assert(eb_register_team(-1,"Alpha","","#11aaff"));
 assert(!eb_register_team(-1,"alpha","","#11aaff"));
 assert(eb_register_team(-1,"Bravo","","#ff11aa"));
 assert(eb_register_team(-1,"Charlie","","#aa11ff"));
 assert(eb_register_team(-1,"Delta","","#11ffaa"));
 assert(!eb_register_team(-1,"Echo","","#11ffaa"));
 assert(eb_start());assert(eb_team_count()==4);
 int a=eb_match_team(0,0,0),b=eb_match_team(0,0,1);assert(a>=0&&b>=0&&a!=b);
 assert(eb_win_probability(a,b)>0&&eb_win_probability(a,b)<1);
 assert(!eb_set_winner(0,0,15));assert(eb_set_winner(0,0,a));assert(eb_set_winner(0,0,a));
 int c=eb_match_team(0,1,0),d=eb_match_team(0,1,1);assert(c>=0&&d>=0);
 assert(eb_set_winner(0,1,c));assert(eb_match_team(1,0,0)==a);
 assert(eb_set_winner(1,0,a));assert(eb_set_winner(1,0,-1));assert(eb_champion()==-1);
 assert(eb_set_winner(1,0,a));assert(eb_champion()==a);
 assert(eb_set_winner(0,0,b));assert(eb_match_winner(1,0)==-1);assert(eb_champion()==-1);assert(eb_set_winner(1,0,b));assert(eb_set_winner(1,0,b));assert(eb_champion()==b);
 double odds[16];assert(eb_simulate(3000,123,odds));double sum=0;for(int i=0;i<16;i++)sum+=odds[i];assert(sum>99.9&&sum<100.1);
 assert(eb_predict(0,0,0,b));assert(eb_set_player_name(0,"Vishal"));assert(eb_score_player(0)>=1);assert(eb_prediction_accuracy(0)>99.9);
 const char*j=eb_export_json();assert(j&&strstr(j,"Test Cup"));char backup[32000];snprintf(backup,sizeof backup,"%.*s,\"oddsHistory\":[{\"odds\":[1.25,3.5]}]}",(int)strlen(j)-1,j);assert(eb_import_json(backup));assert(eb_champion()==b);assert(eb_player_count()==1);assert(eb_score_player(0)>=1);assert(eb_prediction_accuracy(0)>99.9);assert(!eb_import_json("{broken"));assert(eb_champion()==b);
 puts("test_tournament: PASS");return 0;
}
