
#include "engine.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

EBTournament eb;
static char err[160];
const char *eb_error(void){return err;}
static int ci_cmp(const char*a,const char*b){while(*a&&*b){int x=tolower((unsigned char)*a++),y=tolower((unsigned char)*b++);if(x!=y)return x-y;}return (unsigned char)*a-(unsigned char)*b;}
static int valid_text(const char*s,size_t n){if(!s||strlen(s)==0||strlen(s)>=n)return 0;for(const unsigned char*p=(const unsigned char*)s;*p;p++)if(*p<32||*p==34||*p==92)return 0;return 1;}
void eb_reset(void){memset(&eb,0,sizeof eb);eb.rng=0x9e3779b9u;err[0]=0;for(int i=0;i<16;i++)eb.order[i]=-1;for(int i=0;i<15;i++)eb.winners[i]=-1;}
int eb_create(const char*n,const char*g,int size){
 if(size!=4&&size!=8&&size!=16){snprintf(err,sizeof err,"Tournament size must be 4, 8 or 16.");return 0;}
 eb_reset();snprintf(eb.name,sizeof eb.name,"%s",valid_text(n,64)?n:"Open Cup");snprintf(eb.game,sizeof eb.game,"%s",valid_text(g,40)?g:"Esports");eb.size=size;eb.status=1;return 1;
}
int eb_register_team(int id,const char*n,const char*t,const char*c){
 if(eb.status!=1){snprintf(err,sizeof err,"Registration is closed.");return 0;}
 if(!valid_text(n,EB_NAME)){snprintf(err,sizeof err,"Team name is required (1-39 characters).");return 0;}
 if(id < -1){snprintf(err,sizeof err,"Invalid team ID.");return 0;}
 if(id==-1) {for(id=0;id<16&&eb.teams[id].active;id++);}
 if(id>=16){snprintf(err,sizeof err,"Invalid team ID.");return 0;}
 for(int i=0;i<16;i++)if(i!=id&&eb.teams[i].active&&!ci_cmp(eb.teams[i].name,n)){snprintf(err,sizeof err,"Duplicate team name.");return 0;}
 if(!eb.teams[id].active&&eb.count>=eb.size){snprintf(err,sizeof err,"Tournament is full.");return 0;}
 EBTeam *x=&eb.teams[id];if(!x->active)eb.count++;x->id=id;x->active=1;x->rating=x->rating?x->rating:1500;
 snprintf(x->name,sizeof x->name,"%s",n);snprintf(x->tag,sizeof x->tag,"%s",(t&&valid_text(t,EB_TAG))?t:"");
 int color_ok=c&&c[0]=='#'&&strlen(c)==7;for(int k=1;color_ok&&k<7;k++)if(!((c[k]>='0'&&c[k]<='9')||(c[k]>='a'&&c[k]<='f')||(c[k]>='A'&&c[k]<='F')))color_ok=0;
 if(color_ok)snprintf(x->color,sizeof x->color,"%s",c);else snprintf(x->color,sizeof x->color,"#39e8ff");
 return 1;
}
int eb_withdraw(int id){if(eb.status!=1||id<0||id>=16||!eb.teams[id].active){snprintf(err,sizeof err,"Invalid withdrawal.");return 0;}eb.teams[id].active=0;eb.count--;return 1;}
int eb_team_count(void){return eb.count;}
int eb_team_active(int id){return id>=0&&id<16&&eb.teams[id].active;}
const char*eb_team_name(int id){return eb_team_active(id)?eb.teams[id].name:"";}
const char*eb_team_tag(int id){return eb_team_active(id)?eb.teams[id].tag:"";}
const char*eb_team_color(int id){return eb_team_active(id)?eb.teams[id].color:"#718096";}
int eb_team_rating(int id){return eb_team_active(id)?eb.teams[id].rating:0;}
int eb_set_rating(int id,int r){if(!eb_team_active(id)||r<500||r>2500){snprintf(err,sizeof err,"Rating must be 500-2500 for a registered team.");return 0;}eb.teams[id].rating=r;return 1;}
int eb_rounds(void){int n=0,s=eb.size;while(s>1){n++;s/=2;}return n;}
int eb_match_count(int r){return r>=0&&r<eb_rounds()?eb.size>>(r+1):0;}
int eb_match_team(int r,int m,int slot){
 if(r<0||r>=eb_rounds()||m<0||m>=eb_match_count(r)||slot<0||slot>1)return -1;
 int idx=eb.size-(eb.size>>r)+m;
 if(r==0){int id=eb.order[m*2+slot];return eb_team_active(id)?id:-1;}
 int parent=idx;int child=(parent-(eb.size-(eb.size>>r)))*2;
 int source=eb.size-(eb.size>>(r-1))+child+slot;
 return eb.winners[source];
}
int eb_match_winner(int r,int m){if(r<0||r>=eb_rounds()||m<0||m>=eb_match_count(r))return -1;return eb.winners[eb.size-(eb.size>>r)+m];}
int eb_start(void){
 if(eb.status!=1||eb.count!=eb.size){snprintf(err,sizeof err,"Register exactly %d teams before generating the bracket.",eb.size);return 0;}
 int k=0;for(int i=0;i<16;i++)if(eb.teams[i].active)eb.order[k++]=i;
 /* deterministic shuffle */
 for(int i=eb.size-1;i>0;i--){eb.rng=eb.rng*1664525u+1013904223u;int j=(int)(eb.rng%(unsigned)(i+1));int x=eb.order[i];eb.order[i]=eb.order[j];eb.order[j]=x;}
 for(int i=0;i<15;i++)eb.winners[i]=-1;eb.status=2;return 1;
}
int eb_set_winner(int r,int m,int id){
 if(eb.status<2||r<0||r>=eb_rounds()||m<0||m>=eb_match_count(r)){snprintf(err,sizeof err,"Invalid match.");return 0;}
 int idx=eb.size-(eb.size>>r)+m;
 if(id==-1){eb.winners[idx]=-1;for(int rr=r+1;rr<eb_rounds();rr++){int mm=m>>(rr-r);eb.winners[eb.size-(eb.size>>rr)+mm]=-1;}eb.status=2;return 1;}
 int a=eb_match_team(r,m,0),b=eb_match_team(r,m,1);
 if(a<0||b<0||!eb_team_active(id)||(id!=a&&id!=b)){snprintf(err,sizeof err,"Winner must be one of the two active match teams.");return 0;}
 int changed=(eb.winners[idx]!=id);eb.winners[idx]=id;
 if(changed)for(int rr=r+1;rr<eb_rounds();rr++){int mm=m>>(rr-r);eb.winners[eb.size-(eb.size>>rr)+mm]=-1;}
 if(r==eb_rounds()-1)eb.status=3;else eb.status=2;
 return 1;
}
int eb_champion(void){return eb.status==3?eb.winners[eb.size-2]:-1;}
int eb_completed_matches(void){int n=0;for(int i=0;i<eb.size-1;i++)if(eb.winners[i]>=0)n++;return n;}
int eb_team_wins(int id){int n=0;for(int r=0;r<eb_rounds();r++)for(int m=0;m<eb_match_count(r);m++)if(eb_match_winner(r,m)==id)n++;return n;}
int eb_team_losses(int id){int n=0;for(int r=0;r<eb_rounds();r++)for(int m=0;m<eb_match_count(r);m++){int w=eb_match_winner(r,m);if(w>=0&&(eb_match_team(r,m,0)==id||eb_match_team(r,m,1)==id)&&w!=id)n++;}return n;}
double eb_win_probability(int a,int b){if(!eb_team_active(a)||!eb_team_active(b)||a==b)return 0.0;double d=(eb.teams[b].rating-eb.teams[a].rating)/400.0;return 1.0/(1.0+pow(10.0,d));}
