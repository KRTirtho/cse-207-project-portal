#ifndef ADMIN_MODULE_H
#define ADMIN_MODULE_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "cJSON.h"

#define MAX_NAME_LENGTH 100
#define MAX_EMAIL_LENGTH 255
#define MAX_PASSWORD_LENGTH 20
#define MAX_DEPARTMENT_LENGTH 4

typedef struct Admin
{
    uint32_t id;
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
} Admin;

typedef struct AdminNode
{
    Admin *admin;
    struct AdminNode *next;
} AdminNode;


cJSON *admin_to_json(const Admin *a);
Admin *admin_from_json(cJSON *json);
cJSON *admin_list_to_json(const AdminNode *head);
AdminNode *admin_list_from_json(cJSON *json_array);

/* Admin CRUD */
Admin *create_admin(AdminNode **list, uint32_t id, const char *name, const char *email, const char *password);
int delete_admin(AdminNode **list, uint32_t id);
void print_admin_list(AdminNode *list);

/* File I/O */
void save_admins(AdminNode *list, const char *filename);
void load_admins(AdminNode **list, const char *filename);

/* Login */
Admin *admin_login(AdminNode *list);

#endif
