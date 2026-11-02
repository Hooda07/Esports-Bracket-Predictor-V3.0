
#include "engine.h"
#include <math.h>
static unsigned rnd(void){eb.rng=eb.rng*1664525u+1013904223u;return eb.rng;}
static int simulated_winner(int a,int b){double p=eb_win_probability(a,b);return (rnd()/(double)4294967296.0)<p?a:b;}
int eb_simulate(int runs,int seed,double*out){
 if(eb.status<2||runs<1||runs>1000000||!out)return 0;
 if(seed)eb.rng=(unsigned)seed;for(int i=0;i<16;i++)out[i]=0;
 for(int run=0;run<runs;run++){
  int cur[16];for(int i=0;i<eb.size;i++)cur[i]=eb.order[i];
  for(int r=0;r<eb_rounds();r++){for(int m=0;m<eb_match_count(r);m++){
    int idx=eb.size-(eb.size>>r)+m,actual=eb.winners[idx],a=cur[2*m],b=cur[2*m+1];
    int w=actual>=0?actual:simulated_winner(a,b);cur[m]=w;
  }}
  if(cur[0]>=0)out[cur[0]]++;
 }
 for(int i=0;i<16;i++)out[i]=100.0*out[i]/runs;return 1;
}

static int upset_winners[15];
int eb_run_upset(int seed){
 if(eb.status<2)return 0;
 if(seed)eb.rng=(unsigned)seed;
 for(int r=0;r<eb_rounds();r++)for(int m=0;m<eb_match_count(r);m++){
  int a,b;if(r==0){a=eb.order[2*m];b=eb.order[2*m+1];}
  else{int prev=eb.size-(eb.size>>(r-1));a=upset_winners[prev+2*m];b=upset_winners[prev+2*m+1];}
  int idx=eb.size-(eb.size>>r)+m,actual=eb.winners[idx];
  upset_winners[idx]=actual>=0?actual:simulated_winner(a,b);
 }
 return 1;
}
int eb_upset_team(int r,int m,int slot){
 if(r<0||r>=eb_rounds()||m<0||m>=eb_match_count(r)||slot<0||slot>1)return -1;
 if(r==0)return eb.order[2*m+slot];
 return upset_winners[eb.size-(eb.size>>(r-1))+2*m+slot];
}
int eb_upset_winner(int r,int m){
 if(r<0||r>=eb_rounds()||m<0||m>=eb_match_count(r))return -1;
 return upset_winners[eb.size-(eb.size>>r)+m];
}

static int predicted_winners[15];
int eb_run_prediction(void){
 if(eb.status<2)return 0;
 for(int r=0;r<eb_rounds();r++)for(int m=0;m<eb_match_count(r);m++){
  int a,b;if(r==0){a=eb.order[2*m];b=eb.order[2*m+1];}
  else{int prev=eb.size-(eb.size>>(r-1));a=predicted_winners[prev+2*m];b=predicted_winners[prev+2*m+1];}
  int idx=eb.size-(eb.size>>r)+m,actual=eb.winners[idx];
  double p=eb_win_probability(a,b);
  predicted_winners[idx]=actual>=0?actual:(p>=0.5?a:b);
 }
 return 1;
}
int eb_prediction_team(int r,int m,int slot){
 if(r<0||r>=eb_rounds()||m<0||m>=eb_match_count(r)||slot<0||slot>1)return -1;
 if(r==0)return eb.order[2*m+slot];
 return predicted_winners[eb.size-(eb.size>>(r-1))+2*m+slot];
}
int eb_prediction_winner(int r,int m){
 if(r<0||r>=eb_rounds()||m<0||m>=eb_match_count(r))return -1;
 return predicted_winners[eb.size-(eb.size>>r)+m];
}
