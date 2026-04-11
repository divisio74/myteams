/*
** EPITECH PROJECT, 2026
** myteams
** File description:
** job_logic.h
*/

#ifndef JOB_LOGIC_H
  #define JOB_LOGIC_H

#include "mylist.h"
#include <stdbool.h>

typedef struct payload_cmd_s payload_cmd_t;

typedef struct user_s {
    char *name;
    char *uid;
} user_t;

typedef struct message_s {
  char *sender_uid;
  char *receiver_uid;
  char *body;
} message_t;

typedef struct thread_s {
  char *uid;
  char *name;
  char *body;
  char *creator_uid;
  linked_list_t *messages;
} thread_t;

typedef struct channel_s {
  char *uid;
  char *name;
  linked_list_t *users;
  linked_list_t *threads;
} channel_t;


typedef struct team_s {
  char *uid;
  char *name;
  char *description;
  char *creator_uid;
  linked_list_t *subscribers;
  linked_list_t *channels;
} team_t;

typedef struct teams_s {
  linked_list_t *users;
  linked_list_t *messages;
  linked_list_t *teams;
} teams_t;

int compare_user_uid(void *user_data, void *uid_data);
int compare_user_name(void *user_data, void *name_data);
user_t *init_user(const char *name);
user_t *find_or_create_user(teams_t *teams, const char *name);
void free_user(void *data);

int compare_team_uid(void *team_data, void *uid_data);
int compare_string(void *str_a, void *str_b);
team_t *init_team(const char *name, const char *description,
    const char *creator_uid);
void free_team(void *data);

int compare_thread_uid(void *thread_data, void *uid_data);
thread_t *init_thread(const char *name, const char *body,
    const char *creator_uid);
void free_thread(void *data);

int compare_channel_uid(void *channel_data, void *uid_data);
channel_t *init_channel(const char *name);
void free_channel(void *data);

message_t *init_message(const char *sender_uid, const char *receiver_uid,
    const char *body);
void free_message(void *data);
int message_is_exchange_between(message_t *msg,
    const char *uid_a, const char *uid_b);

int teams_init(teams_t *teams);
void teams_free(teams_t *teams);

int cmd_job_help(payload_cmd_t *payload_cmd);
int cmd_job_login(payload_cmd_t *payload_cmd);
int cmd_job_logout(payload_cmd_t *payload_cmd, char **logged_out_uid);
int cmd_job_users(payload_cmd_t *payload_cmd, char **users_output);
int cmd_job_user(payload_cmd_t *payload_cmd, char **user_output);
int cmd_job_send(payload_cmd_t *payload_cmd, const char *receiver_uid,
    const char *body);
int cmd_job_messages(payload_cmd_t *payload_cmd, const char *other_uid,
    linked_list_t **out_filtered);
int cmd_job_subscribe(payload_cmd_t *payload_cmd, const char *team_uid);
int cmd_job_subscribed(payload_cmd_t *payload_cmd, const char *team_uid,
    linked_list_t **out_filtered);
int cmd_job_unsubscribe(payload_cmd_t *payload_cmd, const char *team_uid);
int cmd_job_use(payload_cmd_t *payload_cmd);
int cmd_job_create(payload_cmd_t *payload_cmd, const char *arg1,
    const char *arg2, char **out_uid);
int cmd_job_list(payload_cmd_t *payload_cmd);
int cmd_job_info(payload_cmd_t *payload_cmd);

#endif /* JOB_LOGIC_H */
