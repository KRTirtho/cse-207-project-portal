#include <stdlib.h>
#include <string.h>
#include "admin.h"

static void trim_newline(char *s)
{
    if (!s)
        return;
    size_t n = strlen(s);
    if (n && s[n - 1] == '\n')
        s[n - 1] = '\0';
}

cJSON *admin_to_json(const Admin *a)
{
    if (!a)
        return NULL;
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "id", a->id);
    cJSON_AddStringToObject(json, "name", a->name);
    cJSON_AddStringToObject(json, "email", a->email);
    cJSON_AddStringToObject(json, "password", a->password);
    return json;
}

Admin *admin_from_json(cJSON *json)
{
    Admin *a = calloc(1, sizeof(Admin));
    if (!a)
        return NULL;

    cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
    cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
    cJSON *mail = cJSON_GetObjectItemCaseSensitive(json, "email");
    cJSON *pass = cJSON_GetObjectItemCaseSensitive(json, "password");

    if (cJSON_IsNumber(id))
        a->id = (uint32_t)id->valueint;
    if (cJSON_IsString(name) && name->valuestring)
        snprintf(a->name, sizeof(a->name), "%s", name->valuestring);
    if (cJSON_IsString(mail) && mail->valuestring)
        snprintf(a->email, sizeof(a->email), "%s", mail->valuestring);
    if (cJSON_IsString(pass) && pass->valuestring)
        snprintf(a->password, sizeof(a->password), "%s", pass->valuestring);

    return a;
}

cJSON *admin_list_to_json(const AdminNode *head)
{
    cJSON *array = cJSON_CreateArray();
    for (const AdminNode *cur = head; cur; cur = cur->next)
    {
        if (cur->admin)
        {
            cJSON_AddItemToArray(array, admin_to_json(cur->admin));
        }
    }
    return array;
}

AdminNode *admin_list_from_json(cJSON *json_array)
{
    if (!cJSON_IsArray(json_array))
        return NULL;

    AdminNode *head = NULL, *tail = NULL;
    cJSON *admin_json = NULL;

    cJSON_ArrayForEach(admin_json, json_array)
    {
        Admin *a = admin_from_json(admin_json);
        if (!a)
            continue;

        AdminNode *node = calloc(1, sizeof(AdminNode));
        if (!node)
        {
            free(a);
            continue;
        }
        node->admin = a;
        node->next = NULL;

        if (!head)
            head = node;
        else
            tail->next = node;
        tail = node;
    }

    return head;
}

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
        printf("ID:%u Name:%s Email:%s\n", cur->admin->id, cur->admin->name, cur->admin->email);
        cur = cur->next;
    }
}

void save_admins(AdminNode *list, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp)
        return;

    cJSON *json = admin_list_to_json(list);
    char *out = cJSON_Print(json);

    fputs(out, fp);
    cJSON_Delete(json);
    free(out);
    fclose(fp);
}

void load_admins(AdminNode **list, const char *filename)
{
    char *buffer = 0;
    long length;
    FILE *fp = fopen(filename, "rb");
    if (!fp)
        return;

    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = malloc(length);
    if (buffer)
    {
        fread(buffer, 1, length, fp);
    }
    fclose(fp);

    if (!buffer)
        return;

    cJSON *json = cJSON_Parse(buffer);
    free(buffer);
    if (!json)
        return;
    
    if(cJSON_IsArray(json)) {
        AdminNode *loaded = admin_list_from_json(json);
        // Free existing list
        while (*list) {
            AdminNode *tmp = *list;
            *list = (*list)->next;
            free(tmp->admin);
            free(tmp);
        }
        *list = loaded;
    }
}

Admin *admin_login(AdminNode *list)
{
    char email[MAX_EMAIL_LENGTH], pass[MAX_PASSWORD_LENGTH];

    while (1)
    {
        printf("Admin Login\nEmail: ");
        fgets(email, sizeof(email), stdin);
        email[strcspn(email, "\n")] = 0;

        printf("Password: ");
        fgets(pass, sizeof(pass), stdin);
        pass[strcspn(pass, "\n")] = 0;

        AdminNode *cur = list;
        while (cur)
        {
            if (strcmp(cur->admin->email, email) == 0 && strcmp(cur->admin->password, pass) == 0)
                return cur->admin;
            cur = cur->next;
        }

        printf("Invalid credentials. Please try again.\n\n");
    }
}
