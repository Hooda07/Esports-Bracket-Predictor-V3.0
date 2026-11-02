
#include "engine.h"
#include <string.h>
#include <stdio.h>
int eb_pick_team(int player,int r,int m,int slot){
 if(r<0||r>=eb_rounds()||m<0||m>=eb_match_count(r)||slot<0||slot>1)return -1;
 if(r==0)return eb.order[m*2+slot];
 int prior=2*m+slot,idx=eb.size-(eb.size>>(r-1))+prior;
 if(player>=0&&player<eb.player_count&&eb.players[player].picks[idx]>=0)return eb.players[player].picks[idx];
 return eb.winners[idx];
}
int eb_player_pick(int player,int r,int m){if(player<0||player>=eb.player_count||r<0||r>=eb_rounds()||m<0||m>=eb_match_count(r))return -1;return eb.players[player].picks[eb.size-(eb.size>>r)+m];}
int eb_predict(int player,int r,int m,int team){
 if(eb.status<2||player<0||player>=EB_MAX_PLAYERS||r<0||r>=eb_rounds()||m<0||m>=eb_match_count(r))return 0;
 int a=eb_pick_team(player,r,m,0),b=eb_pick_team(player,r,m,1);if(a<0||b<0||(team!=a&&team!=b))return 0;
 if(player>=eb.player_count){for(int i=eb.player_count;i<=player;i++){memset(&eb.players[i],0,sizeof(EBPlayer));for(int k=0;k<31;k++)eb.players[i].picks[k]=-1;}eb.player_count=player+1;}
 eb.players[player].picks[eb.size-(eb.size>>r)+m]=team;return 1;
}
int eb_score_player(int player){
 if(player<0||player>=eb.player_count)return 0;int score=0;
 for(int r=0;r<eb_rounds();r++)for(int m=0;m<eb_match_count(r);m++){int idx=eb.size-(eb.size>>r)+m,pick=eb.players[player].picks[idx],actual=eb.winners[idx];if(pick>=0&&actual>=0&&pick==actual)score+=(1<<r);}
 return score;
}
int eb_set_player_name(int player,const char*name){if(player<0||player>=EB_MAX_PLAYERS||!name||strlen(name)>31)return 0;if(player>=eb.player_count){for(int i=eb.player_count;i<=player;i++){memset(&eb.players[i],0,sizeof(EBPlayer));for(int k=0;k<31;k++)eb.players[i].picks[k]=-1;}eb.player_count=player+1;}snprintf(eb.players[player].nick,sizeof eb.players[player].nick,"%s",name);return 1;}
double eb_prediction_accuracy(int player){if(player<0||player>=eb.player_count)return 0.0;int total=0,correct=0;for(int r=0;r<eb_rounds();r++)for(int m=0;m<eb_match_count(r);m++){int idx=eb.size-(eb.size>>r)+m,p=eb.players[player].picks[idx],w=eb.winners[idx];if(p>=0&&w>=0){total++;if(p==w)correct++;}}return total?100.0*correct/total:0.0;}
int eb_player_count(void){return eb.player_count;}
const char *eb_player_name(int player){return player>=0&&player<eb.player_count?eb.players[player].nick:"";}
