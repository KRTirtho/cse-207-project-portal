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
            printf(" %u", c->courseID);
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
    for (StudentNode *cur = list; cur; cur = cur->next)
    {
        unsigned advisor_id = cur->student->advisor ? cur->student->advisor->id : 0;
        unsigned count = 0;
        for (StudentCourseNode *c = cur->student->courses; c; c = c->next)
            count++;

        fprintf(fp, "%u|%s|%s|%s|%s|%u|%u|%.2f|%u|%u|",
                cur->student->id,
                cur->student->name,
                cur->student->email,
                cur->student->password,
                cur->student->department,
                cur->student->admission_year,
                cur->student->admission_month,
                cur->student->cgpa,
                advisor_id,
                count);

        unsigned i = 0;
        for (StudentCourseNode *c = cur->student->courses; c; c = c->next, ++i)
        {
            fprintf(fp, "%u", c->courseID);
            if (c->next)
                fputc(',', fp);
        }
        fputc('\n', fp);
    }
    fclose(fp);
}

void load_students(StudentNode **list, const char *filename, FacultyNode *faculty_list)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return;
    char line[2048];
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
        char *year_str = strtok(NULL, "|");
        char *month_str = strtok(NULL, "|");
        char *cgpa_str = strtok(NULL, "|");
        char *adv_str = strtok(NULL, "|");
        char *cnt_str = strtok(NULL, "|");
        char *courses_str = strtok(NULL, "|");

        if (!id_str || !name || !email || !pass || !dept || !year_str || !month_str || !cgpa_str || !adv_str || !cnt_str)
        {
            continue;
        }

        uint32_t id = (uint32_t)strtoul(id_str, NULL, 10);
        uint16_t year = (uint16_t)strtoul(year_str, NULL, 10);
        uint8_t month = (uint8_t)strtoul(month_str, NULL, 10);
        float cgpa = (float)strtod(cgpa_str, NULL);
        uint32_t adv_id = (uint32_t)strtoul(adv_str, NULL, 10);
        unsigned count = (unsigned)strtoul(cnt_str, NULL, 10);

        Faculty *advisor = NULL;
        if (adv_id)
            advisor = find_faculty_by_id(faculty_list, adv_id);

        Student *s = create_student(list, id, name, email, pass, dept, year, month, advisor);
        if (!s)
            continue;
        s->cgpa = cgpa;

        if (courses_str && count > 0)
        {
            char *tok = strtok(courses_str, ",");
            while (tok)
            {
                uint16_t cid = (uint16_t)strtoul(tok, NULL, 10);
                StudentCourseNode *node = (StudentCourseNode *)malloc(sizeof(StudentCourseNode));
                if (!node)
                    break;
                node->courseID = cid;
                node->next = NULL;
                if (!s->courses)
                    s->courses = node;
                else
                {
                    StudentCourseNode *curC = s->courses;
                    while (curC->next)
                        curC = curC->next;
                    curC->next = node;
                }
                tok = strtok(NULL, ",");
            }
        }
    }
    fclose(fp);
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

void student_self_advising(Student *s)
{
}
