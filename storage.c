
#include "engine.h"
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <ctype.h>
static char out[30000];
const char *eb_export_json(void){
 int n=snprintf(out,sizeof out,"{\"version\":3,\"name\":\"%s\",\"game\":\"%s\",\"size\":%d,\"status\":%d,\"teams\":[",eb.name,eb.game,eb.size,eb.status);
 int first=1;for(int i=0;i<16;i++)if(eb.teams[i].active){EBTeam*t=&eb.teams[i];n+=snprintf(out+n,sizeof(out)-n,"%s{\"id\":%d,\"name\":\"%s\",\"tag\":\"%s\",\"color\":\"%s\",\"rating\":%d}",first?"":",",i,t->name,t->tag,t->color,t->rating);first=0;}
 n+=snprintf(out+n,sizeof(out)-n,"],\"order\":[");for(int i=0;i<eb.size;i++)n+=snprintf(out+n,sizeof(out)-n,"%s%d",i?",":"",eb.order[i]);
 n+=snprintf(out+n,sizeof(out)-n,"],\"winners\":[");for(int i=0;i<eb.size-1;i++)n+=snprintf(out+n,sizeof(out)-n,"%s%d",i?",":"",eb.winners[i]);
 n+=snprintf(out+n,sizeof(out)-n,"],\"players\":[");
 for(int p=0;p<eb.player_count;p++){n+=snprintf(out+n,sizeof(out)-n,"%s{\"nick\":\"%s\",\"picks\":[",p?",":"",eb.players[p].nick);for(int k=0;k<eb.size-1;k++)n+=snprintf(out+n,sizeof(out)-n,"%s%d",k?",":"",eb.players[p].picks[k]);n+=snprintf(out+n,sizeof(out)-n,"]}");}
 snprintf(out+n,sizeof(out)-n,"]}");return out;
}
/* Small strict JSON reader for the versioned schema emitted above. */
typedef struct{const char*s;int p;} Parser;
static void ws(Parser*p){while(isspace((unsigned char)p->s[p->p]))p->p++;}
static int ch(Parser*p,char c){ws(p);if(p->s[p->p]!=c)return 0;p->p++;return 1;}
static int str(Parser*p,char*out,int cap){ws(p);if(!ch(p,'"'))return 0;int n=0;while(p->s[p->p]&&p->s[p->p]!='"'){char c=p->s[p->p++];if(c=='\\'){c=p->s[p->p++];if(c=='n')c='\n';else if(c=='t')c='\t';}if(n<cap-1)out[n++]=c;}if(!ch(p,'"'))return 0;out[n]=0;return 1;}
static int num(Parser*p,int*out){ws(p);char*e;long v=strtol(p->s+p->p,&e,10);if(e==p->s+p->p)return 0;p->p=(int)(e-p->s);if(v<-2147483647L||v>2147483647L)return 0;*out=(int)v;return 1;}
static int skip(Parser*p){ws(p);if(p->s[p->p]=='"'){char b[256];return str(p,b,sizeof b);}if(p->s[p->p]=='{'){p->p++;while(1){ws(p);if(ch(p,'}'))return 1;char k[80];if(!str(p,k,80)||!ch(p,':')||!skip(p))return 0;if(ch(p,','))continue;return ch(p,'}');}}if(p->s[p->p]=='['){p->p++;while(1){if(ch(p,']'))return 1;if(!skip(p))return 0;if(ch(p,','))continue;return ch(p,']');}}if(!strncmp(p->s+p->p,"true",4)){p->p+=4;return 1;}if(!strncmp(p->s+p->p,"false",5)){p->p+=5;return 1;}if(!strncmp(p->s+p->p,"null",4)){p->p+=4;return 1;}char*e;strtod(p->s+p->p,&e);if(e==p->s+p->p)return 0;p->p=(int)(e-p->s);return 1;}
static int key(Parser*p,char*k){return str(p,k,80)&&ch(p,':');}
static int ci_cmp(const char*a,const char*b){while(*a&&*b){int x=tolower((unsigned char)*a++),y=tolower((unsigned char)*b++);if(x!=y)return x-y;}return (unsigned char)*a-(unsigned char)*b;}
static int safe_string(const char*s){for(const unsigned char*p=(const unsigned char*)s;*p;p++)if(*p<32||*p==34||*p==92)return 0;return 1;}
static int valid_color(const char*s){if(strlen(s)!=7||s[0]!='#')return 0;for(int i=1;i<7;i++)if(!((s[i]>='0'&&s[i]<='9')||(s[i]>='a'&&s[i]<='f')||(s[i]>='A'&&s[i]<='F')))return 0;return 1;}
int eb_import_json(const char*json){
 if(!json||strlen(json)>250000)return 0;Parser p={json,0};EBTournament tmp;memset(&tmp,0,sizeof tmp);
 for(int i=0;i<16;i++)tmp.order[i]=-1;for(int i=0;i<15;i++)tmp.winners[i]=-1;
 if(!ch(&p,'{'))return 0;int version=0,gotSize=0,gotTeams=0,gotOrder=0,gotWinners=0;
 while(1){ws(&p);if(ch(&p,'}'))break;char k[80];if(!key(&p,k))return 0;
  if(!strcmp(k,"version")){if(!num(&p,&version))return 0;}
  else if(!strcmp(k,"name")){if(!str(&p,tmp.name,sizeof tmp.name))return 0;}
  else if(!strcmp(k,"game")){if(!str(&p,tmp.game,sizeof tmp.game))return 0;}
  else if(!strcmp(k,"size")){if(!num(&p,&tmp.size))return 0;gotSize=1;}
  else if(!strcmp(k,"status")){if(!num(&p,&tmp.status))return 0;}
  else if(!strcmp(k,"teams")){
   if(!ch(&p,'['))return 0;while(!ch(&p,']')){if(!ch(&p,'{'))return 0;EBTeam t={0};int mask=0;while(!ch(&p,'}')){char q[80];if(!key(&p,q))return 0;int v;if(!strcmp(q,"id")){if(!num(&p,&t.id))return 0;mask|=1;}else if(!strcmp(q,"name")){if(!str(&p,t.name,sizeof t.name))return 0;mask|=2;}else if(!strcmp(q,"tag")){if(!str(&p,t.tag,sizeof t.tag))return 0;}else if(!strcmp(q,"color")){if(!str(&p,t.color,sizeof t.color))return 0;}else if(!strcmp(q,"rating")){if(!num(&p,&t.rating))return 0;}else if(!skip(&p))return 0;if(ch(&p,','))continue;ws(&p);if(p.s[p.p]!='}')return 0;}
    if(mask!=3||t.id<0||t.id>=16||tmp.teams[t.id].active||!t.name[0]||t.rating<500||t.rating>2500)return 0;t.active=1;tmp.teams[t.id]=t;tmp.count++;if(ch(&p,','))continue;ws(&p);if(p.s[p.p]!=']')return 0;
   }gotTeams=1;
  }else if(!strcmp(k,"order")||!strcmp(k,"winners")){
   int isOrder=!strcmp(k,"order"),lim=isOrder?16:15;if(!ch(&p,'['))return 0;int i=0;while(!ch(&p,']')){int v;if(i>=lim||!num(&p,&v))return 0;if(isOrder)tmp.order[i]=v;else tmp.winners[i]=v;i++;if(ch(&p,','))continue;ws(&p);if(p.s[p.p]!=']')return 0;}if(isOrder){if(i!=tmp.size)return 0;gotOrder=1;}else{if(i!=tmp.size-1)return 0;gotWinners=1;}
  }else if(!strcmp(k,"players")){
   if(!ch(&p,'['))return 0;
   while(!ch(&p,']')){
    if(tmp.player_count>=EB_MAX_PLAYERS||!ch(&p,'{'))return 0;
    EBPlayer *pl=&tmp.players[tmp.player_count];memset(pl,0,sizeof *pl);
    for(int z=0;z<31;z++)pl->picks[z]=-1;
    while(!ch(&p,'}')){
     char q[80];if(!key(&p,q))return 0;
     if(!strcmp(q,"nick")){if(!str(&p,pl->nick,sizeof pl->nick))return 0;}
     else if(!strcmp(q,"picks")){
      if(!ch(&p,'['))return 0;int z=0;
      while(!ch(&p,']')){int v;if(z>=tmp.size-1||!num(&p,&v)||v<-1||v>=16)return 0;pl->picks[z++]=v;if(ch(&p,','))continue;ws(&p);if(p.s[p.p]!=']')return 0;}
      if(z!=tmp.size-1)return 0;
     }else if(!skip(&p))return 0;
     if(ch(&p,','))continue;ws(&p);if(p.s[p.p]!='}')return 0;
    }
    tmp.player_count++;if(ch(&p,','))continue;ws(&p);if(p.s[p.p]!=']')return 0;
   }
  }else if(!skip(&p))return 0;
  if(ch(&p,','))continue;ws(&p);if(p.s[p.p]!='}')return 0;
 }
 ws(&p);if(p.s[p.p]!=0)return 0;
 if(version!=3||!gotSize||!gotTeams||!gotOrder||!gotWinners||(tmp.size!=4&&tmp.size!=8&&tmp.size!=16)||tmp.count>tmp.size||tmp.status<1||tmp.status>3||!tmp.name[0]||!safe_string(tmp.name)||!safe_string(tmp.game))return 0;
 for(int i=0;i<16;i++)if(tmp.teams[i].active&&(!safe_string(tmp.teams[i].name)||!safe_string(tmp.teams[i].tag)||!valid_color(tmp.teams[i].color)))return 0;
 for(int i=0;i<tmp.player_count;i++)if(!safe_string(tmp.players[i].nick))return 0;
 for(int i=0;i<16;i++)if(tmp.teams[i].active)for(int j=i+1;j<16;j++)if(tmp.teams[j].active&&ci_cmp(tmp.teams[i].name,tmp.teams[j].name)==0)return 0;
 if(tmp.status==1){if(tmp.player_count>0)return 0;for(int i=0;i<tmp.size;i++)if(tmp.order[i]!=-1)return 0;for(int i=0;i<tmp.size-1;i++)if(tmp.winners[i]!=-1)return 0;}
 if(tmp.status>=2){if(tmp.count!=tmp.size)return 0;
  int seen[16]={0};for(int i=0;i<tmp.size;i++){int id=tmp.order[i];if(id<0||id>=16||!tmp.teams[id].active||seen[id])return 0;seen[id]=1;}
  for(int r=0;r<4;r++){if(r>= (tmp.size==4?2:tmp.size==8?3:4))break;int count=tmp.size>>(r+1),start=tmp.size-(tmp.size>>r);
   for(int m=0;m<count;m++){int w=tmp.winners[start+m];if(w < -1 || w>=16)return 0;if(w<0)continue;int a,b;if(r==0){a=tmp.order[2*m];b=tmp.order[2*m+1];}else{int ps=tmp.size-(tmp.size>>(r-1));a=tmp.winners[ps+2*m];b=tmp.winners[ps+2*m+1];}if(w!=a&&w!=b)return 0;}
  }
  for(int pi=0;pi<tmp.player_count;pi++)for(int r=0;r<(tmp.size==4?2:tmp.size==8?3:4);r++)for(int m=0;m<(tmp.size>>(r+1));m++){
    int pick=tmp.players[pi].picks[tmp.size-(tmp.size>>r)+m];if(pick < -1 || pick>=16)return 0;if(pick<0)continue;
    int a,b;if(r==0){a=tmp.order[2*m];b=tmp.order[2*m+1];}else{int prev=tmp.size-(tmp.size>>(r-1)),ia=prev+2*m,ib=prev+2*m+1;
      a=tmp.players[pi].picks[ia]>=0?tmp.players[pi].picks[ia]:tmp.winners[ia];
      b=tmp.players[pi].picks[ib]>=0?tmp.players[pi].picks[ib]:tmp.winners[ib];}
    if(pick!=a&&pick!=b)return 0;
  }
  int final=tmp.winners[tmp.size-2];if((tmp.status==3)!=(final>=0))return 0;
 }
 tmp.rng=0x9e3779b9u;eb=tmp;return 1;
}
const char*eb_state_json(void){return eb_export_json();}
