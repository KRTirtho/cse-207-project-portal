#include <stdlib.h>
#include <string.h>

#include "./faculty.h"

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
Faculty *create_faculty(FacultyNode **list, uint32_t id, const char *name, const char *email, const char *password, const char *dept)
{
    Faculty *f = (Faculty *)malloc(sizeof(Faculty));
    if (!f)
        return NULL;
    f->id = id;
    strncpy(f->name, name ? name : "", MAX_NAME_LENGTH);
    f->name[MAX_NAME_LENGTH - 1] = '\0';
    strncpy(f->email, email ? email : "", MAX_EMAIL_LENGTH);
    f->email[MAX_EMAIL_LENGTH - 1] = '\0';
    strncpy(f->password, password ? password : "", MAX_PASSWORD_LENGTH);
    f->password[MAX_PASSWORD_LENGTH - 1] = '\0';
    strncpy(f->department, dept ? dept : "", MAX_DEPARTMENT_LENGTH);
    f->department[MAX_DEPARTMENT_LENGTH - 1] = '\0';
    f->next = NULL;

    FacultyNode *node = (FacultyNode *)malloc(sizeof(FacultyNode));
    if (!node)
    {
        free(f);
        return NULL;
    }
    node->faculty = f;
    node->next = NULL;

    if (*list == NULL)
        *list = node;
    else
    {
        FacultyNode *cur = *list;
        while (cur->next)
            cur = cur->next;
        cur->next = node;
    }
    return f;
}

int delete_faculty(FacultyNode **list, uint32_t id)
{
    FacultyNode *cur = *list, *prev = NULL;
    while (cur)
    {
        if (cur->faculty->id == id)
        {
            if (prev)
                prev->next = cur->next;
            else
                *list = cur->next;
            free(cur->faculty);
            free(cur);
            return 0;
        }
        prev = cur;
        cur = cur->next;
    }
    return -1;
}

void print_faculty_list(FacultyNode *list)
{
    FacultyNode *cur = list;
    while (cur)
    {
        printf("ID:%u Name:%s Dept:%s\n",
               cur->faculty->id, cur->faculty->name, cur->faculty->department);
        cur = cur->next;
    }
}

Faculty *find_faculty_by_id(FacultyNode *list, uint32_t id)
{
    for (FacultyNode *cur = list; cur; cur = cur->next)
    {
        if (cur->faculty->id == id)
            return cur->faculty;
    }
    return NULL;
}

/* ---------- File I/O ---------- */
/* Format (one faculty per line):
   id|name|email|password|dept
*/
void save_faculty(FacultyNode *list, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp)
        return;
    for (FacultyNode *cur = list; cur; cur = cur->next)
    {
        fprintf(fp, "%u|%s|%s|%s|%s\n",
                cur->faculty->id,
                cur->faculty->name,
                cur->faculty->email,
                cur->faculty->password,
                cur->faculty->department);
    }
    fclose(fp);
}

void load_faculty(FacultyNode **list, const char *filename)
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
        char *dept = strtok(NULL, "|");
        if (!id_str || !name || !email || !pass || !dept)
            continue;
        uint32_t id = (uint32_t)strtoul(id_str, NULL, 10);
        create_faculty(list, id, name, email, pass, dept);
    }
    fclose(fp);
}
