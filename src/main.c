
#include "engine.h"
#include <string.h>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#define API EMSCRIPTEN_KEEPALIVE
#else
#define API
#endif
API void api_reset(void){eb_reset();}
API int api_create(const char*n,const char*g,int s){return eb_create(n,g,s);}
API int api_register(int id,const char*n,const char*t,const char*c){return eb_register_team(id,n,t,c);}
API int api_withdraw(int id){return eb_withdraw(id);}
API int api_start(void){return eb_start();}
API int api_winner(int r,int m,int id){return eb_set_winner(r,m,id);}
API int api_rating(int id,int r){return eb_set_rating(id,r);}
API double api_probability(int a,int b){return eb_win_probability(a,b);}
API int api_simulate(int runs,int seed,double*out){return eb_simulate(runs,seed,out);}
API int api_upset_run(int seed){return eb_run_upset(seed);}
API int api_run_prediction(void){return eb_run_prediction();}
API int api_prediction_team(int r,int m,int s){return eb_prediction_team(r,m,s);}
API int api_prediction_winner(int r,int m){return eb_prediction_winner(r,m);}
API int api_upset_team(int r,int m,int s){return eb_upset_team(r,m,s);}
API int api_upset_winner(int r,int m){return eb_upset_winner(r,m);}
API int api_pick(int p,int r,int m,int t){return eb_predict(p,r,m,t);}
API int api_pick_team(int p,int r,int m,int s){return eb_pick_team(p,r,m,s);}
API int api_player_pick(int p,int r,int m){return eb_player_pick(p,r,m);}
API int api_score(int p){return eb_score_player(p);}
API int api_player_name(int p,const char*n){return eb_set_player_name(p,n);}
API int api_players_count(void){return eb_player_count();}
API const char*api_player_nick(int p){return eb_player_name(p);}
API int api_rank(int p){return eb_leaderboard_rank(p);}
API const char*api_export(void){return eb_export_json();}
API int api_import(const char*s){return eb_import_json(s);}
API const char*api_error(void){return eb_error();}
API const char*api_state(void){return eb_state_json();}
API int api_status(void){return eb.status;}
API int api_size(void){return eb.size;}
API int api_count(void){return eb_team_count();}
API int api_active(int id){return eb_team_active(id);}
API const char*api_team_name(int id){return eb_team_name(id);}
API const char*api_team_tag(int id){return eb_team_tag(id);}
API const char*api_team_color(int id){return eb_team_color(id);}
API int api_team_rating(int id){return eb_team_rating(id);}
API int api_rounds(void){return eb_rounds();}
API int api_match_count(int r){return eb_match_count(r);}
API int api_match_team(int r,int m,int s){return eb_match_team(r,m,s);}
API int api_match_winner(int r,int m){return eb_match_winner(r,m);}
API int api_champion(void){return eb_champion();}
API int api_completed(void){return eb_completed_matches();}
API int api_team_wins(int id){return eb_team_wins(id);}
API int api_team_losses(int id){return eb_team_losses(id);}
API double api_accuracy(int player){return eb_prediction_accuracy(player);}
