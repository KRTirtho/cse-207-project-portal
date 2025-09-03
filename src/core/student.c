#include <stdlib.h>
#include <string.h>

#include "student.h"

/* ---------- Internal helpers ---------- */
static void trim_newline(char *s)
{
    if (!s)
        return;
    size_t n = strlen(s);
    if (n && s[n - 1] == '\n')
        s[n - 1] = '\0';
}

static void free_courses(StudentCourseNode *c)
{
    while (c)
    {
        StudentCourseNode *tmp = c;
        c = c->next;
        free(tmp);
    }
}

StudentCourseNode *create_student_course(const char *code, uint8_t section)
{
    StudentCourseNode *node = (StudentCourseNode *)malloc(sizeof(StudentCourseNode));
    if (!node)
        return NULL;
    strncpy(node->code, code ? code : "", MAX_COURSE_CODE_LENGTH);
    node->code[MAX_COURSE_CODE_LENGTH - 1] = '\0';
    node->section = section;
    node->next = NULL;
    return node;
}

void push_student_course(Student *s, const char *code, uint8_t section)
{
    if (!s)
        return;
    StudentCourseNode *node = create_student_course(code, section);
    if (!node)
        return;
    node->next = s->courses;
    s->courses = node;
}

void remove_student_course(Student *s, const char *code, uint8_t section)
{
    if (!s || !code)
        return;

    StudentCourseNode *cur = s->courses;
    StudentCourseNode *prev = NULL;
    while (cur)
    {
        if (strcmp(cur->code, code) == 0 && cur->section == section)
        {
            if (prev)
                prev->next = cur->next;
            else
                s->courses = cur->next;
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

cJSON *student_course_to_json(const StudentCourseNode *course)
{
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "code", course->code);
    cJSON_AddNumberToObject(json, "section", course->section);
    return json;
}

StudentCourseNode *student_courses_from_json(cJSON *json_array)
{
    StudentCourseNode *head = NULL, *tail = NULL;
    cJSON *course_json = NULL;

    cJSON_ArrayForEach(course_json, json_array)
    {
        StudentCourseNode *node = calloc(1, sizeof(StudentCourseNode));
        if (!node)
            continue;

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

// --- Student ---

cJSON *student_to_json(const Student *s)
{
    if (!s)
        return NULL;
    cJSON *json = cJSON_CreateObject();

    cJSON_AddNumberToObject(json, "id", s->id);
    cJSON_AddStringToObject(json, "name", s->name);
    cJSON_AddStringToObject(json, "email", s->email);
    cJSON_AddStringToObject(json, "password", s->password);
    cJSON_AddStringToObject(json, "department", s->department);
    cJSON_AddNumberToObject(json, "admission_year", s->admission_year);
    cJSON_AddNumberToObject(json, "admission_month", s->admission_month);
    cJSON_AddNumberToObject(json, "cgpa", s->cgpa);

    cJSON *courses_array = cJSON_CreateArray();
    for (StudentCourseNode *cur = s->courses; cur; cur = cur->next)
    {
        cJSON_AddItemToArray(courses_array, student_course_to_json(cur));
    }
    cJSON_AddItemToObject(json, "courses", courses_array);

    if (s->advisor)
    {
        extern cJSON *faculty_to_json(const struct Faculty *f);
        cJSON_AddItemToObject(json, "advisor", faculty_to_json(s->advisor));
    }
    else
    {
        cJSON_AddNullToObject(json, "advisor");
    }

    return json;
}

Student *student_from_json(cJSON *json)
{
    Student *s = calloc(1, sizeof(Student));
    if (!s)
        return NULL;

    cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
    cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
    cJSON *email = cJSON_GetObjectItemCaseSensitive(json, "email");
    cJSON *pass = cJSON_GetObjectItemCaseSensitive(json, "password");
    cJSON *dept = cJSON_GetObjectItemCaseSensitive(json, "department");
    cJSON *year = cJSON_GetObjectItemCaseSensitive(json, "admission_year");
    cJSON *month = cJSON_GetObjectItemCaseSensitive(json, "admission_month");
    cJSON *cgpa = cJSON_GetObjectItemCaseSensitive(json, "cgpa");
    cJSON *courses = cJSON_GetObjectItemCaseSensitive(json, "courses");
    cJSON *adv = cJSON_GetObjectItemCaseSensitive(json, "advisor");

    if (cJSON_IsNumber(id))
        s->id = (uint32_t)id->valueint;
    if (cJSON_IsString(name) && name->valuestring)
        snprintf(s->name, sizeof(s->name), "%s", name->valuestring);
    if (cJSON_IsString(email) && email->valuestring)
        snprintf(s->email, sizeof(s->email), "%s", email->valuestring);
    if (cJSON_IsString(pass) && pass->valuestring)
        snprintf(s->password, sizeof(s->password), "%s", pass->valuestring);
    if (cJSON_IsString(dept) && dept->valuestring)
        snprintf(s->department, sizeof(s->department), "%s", dept->valuestring);
    if (cJSON_IsNumber(year))
        s->admission_year = (uint16_t)year->valueint;
    if (cJSON_IsNumber(month))
        s->admission_month = (uint8_t)month->valueint;
    if (cJSON_IsNumber(cgpa))
        s->cgpa = (float)cgpa->valuedouble;

    if (cJSON_IsArray(courses))
    {
        s->courses = student_courses_from_json(courses);
    }

    if (cJSON_IsObject(adv))
    {
        extern struct Faculty *faculty_from_json(cJSON * json);
        s->advisor = faculty_from_json(adv);
    }

    return s;
}

/* ---------- CRUD ---------- */
Student *create_student(StudentNode **list, uint32_t id, const char *name, const char *email, const char *password,
                        const char *dept, uint16_t year, uint8_t month, Faculty *advisor)
{
    Student *s = (Student *)malloc(sizeof(Student));
    if (!s)
        return NULL;
    s->id = id;
    strncpy(s->name, name ? name : "", MAX_NAME_LENGTH);
    s->name[MAX_NAME_LENGTH - 1] = '\0';
    strncpy(s->email, email ? email : "", MAX_EMAIL_LENGTH);
    s->email[MAX_EMAIL_LENGTH - 1] = '\0';
    strncpy(s->password, password ? password : "", MAX_PASSWORD_LENGTH);
    s->password[MAX_PASSWORD_LENGTH - 1] = '\0';
    strncpy(s->department, dept ? dept : "", MAX_DEPARTMENT_LENGTH);
    s->department[MAX_DEPARTMENT_LENGTH - 1] = '\0';
    s->admission_year = year;
    s->admission_month = month;
    s->cgpa = 0.0f;
    s->advisor = advisor;
    s->courses = NULL;
    s->next = NULL;

    StudentNode *node = (StudentNode *)malloc(sizeof(StudentNode));
    if (!node)
    {
        free(s);
        return NULL;
    }
    node->student = s;
    node->next = NULL;

    if (*list == NULL)
        *list = node;
    else
    {
        StudentNode *cur = *list;
        while (cur->next)
            cur = cur->next;
        cur->next = node;
    }
    return s;
}

int delete_student(StudentNode **list, uint32_t id)
{
    StudentNode *cur = *list, *prev = NULL;
    while (cur)
    {
        if (cur->student->id == id)
        {
            if (prev)
                prev->next = cur->next;
            else
                *list = cur->next;
            free_courses(cur->student->courses);
            free(cur->student);
            free(cur);
            return 0;
        }
        prev = cur;
        cur = cur->next;
    }
    return -1;
}

void print_student_list(StudentNode *list)
{
    StudentNode *cur = list;
    while (cur)
    {
        printf("ID:%u Name:%s Dept:%s Year:%u Month:%u CGPA:%.2f",
               cur->student->id, cur->student->name, cur->student->department,
               cur->student->admission_year, cur->student->admission_month,
               cur->student->cgpa);
        if (cur->student->advisor)
            printf(" AdvisorID:%u", cur->student->advisor->id);
        printf("\n  Courses:");
        StudentCourseNode *c = cur->student->courses;
        if (!c)
            printf(" (none)");
        while (c)
        {
            printf(" Code: %s Sec: %d", c->code, c->section);
            c = c->next;
        }
        printf("\n");
        cur = cur->next;
    }
}

/* ---------- File I/O ---------- */
/* Format per line:
   id|name|email|password|dept|year|month|cgpa|advisor_id|courseCount|c1,c2,c3
   - advisor_id may be 0 for NULL
   - if courseCount == 0, the last field may be empty or omitted
*/
void save_students(StudentNode *list, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp)
        return;
    cJSON *array = cJSON_CreateArray();

    for (StudentNode *cur = list; cur; cur = cur->next)
    {
        cJSON *json = student_to_json(cur->student);
        cJSON_AddItemToArray(array, json);
    }

    char *out = cJSON_Print(array);
    if (out)
    {
        fprintf(fp, "%s\n", out);
        free(out);
    }

    cJSON_Delete(array);
    fclose(fp);
}

void load_students(StudentNode **list, const char *filename, FacultyNode *faculty_list)
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

    if (cJSON_IsArray(json))
    {
        for (int i = 0; i < cJSON_GetArraySize(json); i++)
        {
            cJSON *item = cJSON_GetArrayItem(json, i);
            Student *s = student_from_json(item);
            if (s)
            {
                // Add the student to the list
                StudentNode *node = (StudentNode *)malloc(sizeof(StudentNode));
                if (node)
                {
                    node->student = s;
                    node->next = *list;
                    *list = node;
                }
            }
        }
    }
}

Student *student_login(StudentNode *list)
{
    char email[MAX_EMAIL_LENGTH], pass[MAX_PASSWORD_LENGTH];

    while (1)
    {
        printf("Student Login\nEmail: ");
        fgets(email, sizeof(email), stdin);
        email[strcspn(email, "\n")] = 0;

        printf("Password: ");
        fgets(pass, sizeof(pass), stdin);
        pass[strcspn(pass, "\n")] = 0;
        StudentNode *cur = list;
        while (cur)
        {
            if (strcmp(cur->student->email, email) == 0 && strcmp(cur->student->password, pass) == 0)
                return cur->student;
            cur = cur->next;
        }
        printf("Invalid credentials. Please try again.\n\n");
    }
}