
#include "../src/engine.h"
#include <assert.h>
#include <stdio.h>
int main(void){eb_reset();assert(eb_create("16 Cup","Game",16));for(int i=0;i<16;i++){char n[20];sprintf(n,"Squad %d",i);assert(eb_register_team(-1,n,"","#39e8ff"));}assert(eb_start());assert(eb_rounds()==4);for(int r=0;r<4;r++)for(int m=0;m<eb_match_count(r);m++){int a=eb_match_team(r,m,0),b=eb_match_team(r,m,1);assert(a>=0&&b>=0);assert(eb_set_winner(r,m,a));}assert(eb_champion()>=0);eb_reset();assert(eb_create("Eight Cup","Game",8));for(int i=0;i<8;i++){char n[20];sprintf(n,"Eight %d",i);assert(eb_register_team(-1,n,"","#39e8ff"));}assert(eb_start());for(int r=0;r<3;r++)for(int m=0;m<eb_match_count(r);m++)assert(eb_set_winner(r,m,eb_match_team(r,m,0)));assert(eb_champion()>=0);puts("test_bracket: PASS");}
