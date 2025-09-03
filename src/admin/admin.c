#include <stdlib.h>
#include <string.h>

#include "./admin.h"
/* ---------- Internal helpers ---------- */
static void trim_newline(char *s)
{
    if (!s)
        return;
    size_t n = strlen(s);
    if (n && s[n - 1] == '\n')
        s[n - 1] = '\0';
}

/* ---------- CRUD ---------- */
Admin *create_admin(AdminNode **list, uint32_t id, const char *name, const char *email, const char *password)
{
    Admin *a = (Admin *)malloc(sizeof(Admin));
    if (!a)
        return NULL;
    a->id = id;
    strncpy(a->name, name ? name : "", MAX_NAME_LENGTH);
    a->name[MAX_NAME_LENGTH - 1] = '\0';
    strncpy(a->email, email ? email : "", MAX_EMAIL_LENGTH);
    a->email[MAX_EMAIL_LENGTH - 1] = '\0';
    strncpy(a->password, password ? password : "", MAX_PASSWORD_LENGTH);
    a->password[MAX_PASSWORD_LENGTH - 1] = '\0';

    AdminNode *node = (AdminNode *)malloc(sizeof(AdminNode));
    if (!node)
    {
        free(a);
        return NULL;
    }
    node->admin = a;
    node->next = NULL;

    if (*list == NULL)
        *list = node;
    else
    {
        AdminNode *cur = *list;
        while (cur->next)
            cur = cur->next;
        cur->next = node;
    }
    return a;
}

int delete_admin(AdminNode **list, uint32_t id)
{
    AdminNode *cur = *list, *prev = NULL;
    while (cur)
    {
        if (cur->admin->id == id)
        {
            if (prev)
                prev->next = cur->next;
            else
                *list = cur->next;
            free(cur->admin);
            free(cur);
            return 0;
        }
        prev = cur;
        cur = cur->next;
    }
    return -1;
}

void print_admin_list(AdminNode *list)
{
    AdminNode *cur = list;
    while (cur)
    {
        printf("ID:%u Name:%s Email:%s\n",
               cur->admin->id, cur->admin->name, cur->admin->email);
        cur = cur->next;
    }
}

/* ---------- File I/O ---------- */
/* Format (one admin per line):
   id|name|email|password
*/
void save_admins(AdminNode *list, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp)
        return;
    for (AdminNode *cur = list; cur; cur = cur->next)
    {
        fprintf(fp, "%u|%s|%s|%s\n",
                cur->admin->id,
                cur->admin->name,
                cur->admin->email,
                cur->admin->password);
    }
    fclose(fp);
}

void load_admins(AdminNode **list, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return;
    char line[1024];
    while (fgets(line, sizeof(line), fp))
    {
        trim_newline(line);
        if (!line[0])
            continue;
        char *id_str = strtok(line, "|");
        char *name = strtok(NULL, "|");
        char *email = strtok(NULL, "|");
        char *pass = strtok(NULL, "|");
        if (!id_str || !name || !email || !pass)
            continue;
        uint32_t id = (uint32_t)strtoul(id_str, NULL, 10);
        create_admin(*&list, id, name, email, pass); // append
    }
    fclose(fp);
}
