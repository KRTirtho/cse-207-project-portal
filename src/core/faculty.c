#include <stdlib.h>
#include <string.h>
#include "faculty.h"

static void trim_newline(char *s)
{
    if (!s)
        return;
    size_t n = strlen(s);
    if (n && s[n - 1] == '\n')
        s[n - 1] = '\0';
}

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

void print_faculty(Faculty *faculty)
{
    printf("ID:%u Name:%s Dept:%s", faculty->id, faculty->name, faculty->department);
    printf(" Courses: ");
    FacultyCourseNode *course = faculty->courses;
    while (course)
    {
        printf("%s (Section %d) ", course->code, course->section);
        course = course->next;
    }
    printf("\n");
}

void print_faculty_list(FacultyNode *list)
{
    FacultyNode *cur = list;
    while (cur)
    {
        print_faculty(cur->faculty);
        printf("\n");
        cur = cur->next;
    }
}

FacultyCourseNode *create_faculty_course(const char *code, uint8_t section)
{
    FacultyCourseNode *node = (FacultyCourseNode *)malloc(sizeof(FacultyCourseNode));
    if (!node)
        return NULL;
    strncpy(node->code, code ? code : "", MAX_COURSE_CODE_LENGTH);
    node->code[MAX_COURSE_CODE_LENGTH - 1] = '\0';
    node->section = section;
    node->next = NULL;
    return node;
}

void push_faculty_course(Faculty *f, const char *code, uint8_t section)
{
    if (!f)
        return;
    FacultyCourseNode *node = create_faculty_course(code, section);
    if (!node)
        return;
    node->next = f->courses;
    f->courses = node;
}

void remove_faculty_course(Faculty *faculty, const char *code, uint8_t section)
{
    if (!faculty || !code)
        return;

    FacultyCourseNode *cur = faculty->courses;
    FacultyCourseNode *prev = NULL;
    while (cur)
    {
        if (strcmp(cur->code, code) == 0 && cur->section == section)
        {
            if (prev)
                prev->next = cur->next;
            else
                faculty->courses = cur->next;
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

cJSON *faculty_course_to_json(const FacultyCourseNode *course)
{
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "code", course->code);
    cJSON_AddNumberToObject(json, "section", course->section);
    return json;
}

cJSON *faculty_to_json(const Faculty *f)
{
    if (!f)
        return NULL;

    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "id", f->id);
    cJSON_AddStringToObject(json, "name", f->name);
    cJSON_AddStringToObject(json, "email", f->email);
    cJSON_AddStringToObject(json, "password", f->password);
    cJSON_AddStringToObject(json, "department", f->department);

    // Convert linked list of courses into JSON array
    cJSON *courses_array = cJSON_CreateArray();
    FacultyCourseNode *cur = f->courses;
    while (cur)
    {
        cJSON_AddItemToArray(courses_array, faculty_course_to_json(cur));
        cur = cur->next;
    }
    cJSON_AddItemToObject(json, "courses", courses_array);

    return json;
}

FacultyCourseNode *faculty_courses_from_json(cJSON *json_array)
{
    FacultyCourseNode *head = NULL, *tail = NULL;

    cJSON *course_json = NULL;
    cJSON_ArrayForEach(course_json, json_array)
    {
        FacultyCourseNode *node = create_faculty_course("", 0);

        cJSON *code = cJSON_GetObjectItemCaseSensitive(course_json, "code");
        cJSON *section = cJSON_GetObjectItemCaseSensitive(course_json, "section");

        if (cJSON_IsString(code) && code->valuestring)
            snprintf(node->code, sizeof(node->code), "%s", code->valuestring);

        if (cJSON_IsNumber(section))
            node->section = (uint8_t)section->valueint;

        node->next = NULL;

        if (!head)
            head = node;
        else
            tail->next = node;
        tail = node;
    }
    return head;
}

Faculty *faculty_from_json(cJSON *json)
{
    Faculty *f = malloc(sizeof(Faculty));
    if (!f)
        return NULL;

    cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
    cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
    cJSON *email = cJSON_GetObjectItemCaseSensitive(json, "email");
    cJSON *password = cJSON_GetObjectItemCaseSensitive(json, "password");
    cJSON *department = cJSON_GetObjectItemCaseSensitive(json, "department");
    cJSON *courses = cJSON_GetObjectItemCaseSensitive(json, "courses");

    if (cJSON_IsNumber(id))
        f->id = (uint32_t)id->valuedouble;
    if (cJSON_IsString(name) && name->valuestring)
        snprintf(f->name, sizeof(f->name), "%s", name->valuestring);
    if (cJSON_IsString(email) && email->valuestring)
        snprintf(f->email, sizeof(f->email), "%s", email->valuestring);
    if (cJSON_IsString(password) && password->valuestring)
        snprintf(f->password, sizeof(f->password), "%s", password->valuestring);
    if (cJSON_IsString(department) && department->valuestring)
        snprintf(f->department, sizeof(f->department), "%s", department->valuestring);

    if (cJSON_IsArray(courses))
    {
        f->courses = faculty_courses_from_json(courses);
    }

    return f;
}

Faculty *find_faculty_by_id(FacultyNode *list, uint32_t id)
{
    for (FacultyNode *cur = list; cur; cur = cur->next)
        if (cur->faculty->id == id)
            return cur->faculty;
    return NULL;
}

void save_faculty(FacultyNode *list, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp)
        return;

    cJSON *array = cJSON_CreateArray();
    for (FacultyNode *cur = list; cur; cur = cur->next)
    {
        cJSON_AddItemToArray(array, faculty_to_json(cur->faculty));
    }

    char *json_string = cJSON_Print(array);
    if (json_string)
    {
        fprintf(fp, "%s\n", json_string);
        free(json_string);
    }
    cJSON_Delete(array);
    fclose(fp);
}

void load_faculty(FacultyNode **list, const char *filename)
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

    // Parse array of faculties and convert to linked list
    if (cJSON_IsArray(json))
    {
        cJSON *faculty_json = NULL;
        cJSON_ArrayForEach(faculty_json, json)
        {
            Faculty *f = faculty_from_json(faculty_json);
            if (f)
            {
                FacultyNode *node = (FacultyNode *)malloc(sizeof(FacultyNode));
                if (node)
                {
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
                }
                else
                {
                    free(f);
                }
            }
        }
    }
}

Faculty *faculty_login(FacultyNode *list)
{
    char email[MAX_EMAIL_LENGTH], pass[MAX_PASSWORD_LENGTH];

    while (1)
    {
        printf("Faculty Login\nEmail: ");
        fgets(email, sizeof(email), stdin);
        email[strcspn(email, "\n")] = 0;

        printf("Password: ");
        fgets(pass, sizeof(pass), stdin);
        pass[strcspn(pass, "\n")] = 0;
        FacultyNode *cur = list;

        while (cur)
        {
            if (strcmp(cur->faculty->email, email) == 0 && strcmp(cur->faculty->password, pass) == 0)
                return cur->faculty;
            cur = cur->next;
        }

        printf("Invalid credentials. Please try again.\n\n");
    }
}
