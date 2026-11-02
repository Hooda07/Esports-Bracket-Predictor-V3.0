
#ifndef ESPORTS_ENGINE_H
#define ESPORTS_ENGINE_H
#include <stddef.h>
#include <stdint.h>
#define EB_MAX_TEAMS 16
#define EB_MAX_PLAYERS 32
#define EB_NAME 40
#define EB_TAG 48
#define EB_COLOR 10
#define EB_MAX_ROUNDS 4
typedef struct { int id, active; char name[EB_NAME], tag[EB_TAG], color[EB_COLOR]; int rating; } EBTeam;
typedef struct { char nick[32]; int picks[31]; int used; } EBPlayer;
typedef struct {
 char name[64], game[40]; int size, status, count, order[16], winners[15];
 EBTeam teams[16]; EBPlayer players[EB_MAX_PLAYERS]; int player_count;
 unsigned int rng;
} EBTournament;
extern EBTournament eb;
void eb_reset(void);
int eb_create(const char*,const char*,int);
int eb_register_team(int,const char*,const char*,const char*);
int eb_withdraw(int);
int eb_start(void);
int eb_set_winner(int,int,int);
int eb_set_rating(int,int);
double eb_win_probability(int,int);
int eb_rounds(void);
int eb_match_count(int);
int eb_match_team(int,int,int);
int eb_match_winner(int,int);
int eb_simulate(int,int,double*);
int eb_run_upset(int);
int eb_run_prediction(void);
int eb_prediction_team(int,int,int);
int eb_prediction_winner(int,int);
int eb_upset_team(int,int,int);
int eb_upset_winner(int,int);
int eb_predict(int,int,int,int);
int eb_pick_team(int,int,int,int);
int eb_player_pick(int,int,int);
int eb_score_player(int);
int eb_set_player_name(int,const char*);
int eb_player_count(void);
const char *eb_player_name(int);
int eb_leaderboard_rank(int);
const char *eb_team_name(int);
const char *eb_team_tag(int);
const char *eb_team_color(int);
int eb_team_rating(int);
int eb_team_active(int);
int eb_team_count(void);
int eb_champion(void);
int eb_completed_matches(void);
int eb_team_wins(int);
int eb_team_losses(int);
double eb_prediction_accuracy(int);
const char *eb_export_json(void);
int eb_import_json(const char*);
const char *eb_error(void);
const char *eb_state_json(void);
#endif
