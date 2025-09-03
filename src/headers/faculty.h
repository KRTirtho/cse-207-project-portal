#ifndef FACULTY_H
#define FACULTY_H

#include <stdint.h>
#include <stdio.h>
#include "admin.h"
#include "cJSON.h"

#define MAX_COURSE_CODE_LENGTH 5

typedef struct Faculty
{
    uint32_t id;
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char department[MAX_DEPARTMENT_LENGTH];
    struct Faculty *next;
    struct FacultyCourseNode *courses;
} Faculty;

typedef struct FacultyNode
{
    Faculty *faculty;
    struct FacultyNode *next;
} FacultyNode;

typedef struct FacultyCourseNode
{
    char code[MAX_COURSE_CODE_LENGTH];
    uint8_t section;
    struct FacultyCourseNode *next;
} FacultyCourseNode;

FacultyCourseNode *create_faculty_course(const char *code, uint8_t section);
void push_faculty_course(Faculty *f, const char *code, uint8_t section);
void remove_faculty_course(Faculty *f, const char *code, uint8_t section);

cJSON *faculty_course_to_json(const FacultyCourseNode *course);
cJSON *faculty_to_json(const Faculty *f);
FacultyCourseNode *faculty_courses_from_json(cJSON *json_array);
Faculty *faculty_from_json(cJSON *json);

/* Faculty CRUD */
Faculty *create_faculty(FacultyNode **list, uint32_t id, const char *name, const char *email, const char *password, const char *dept);
int delete_faculty(FacultyNode **list, uint32_t id);
void print_faculty_list(FacultyNode *list);

void add_faculty_course(Faculty *faculty, const char *code, uint8_t section);
void remove_faculty_course(Faculty *faculty, const char *code, uint8_t section);

/* Helpers */
Faculty *find_faculty_by_id(FacultyNode *list, uint32_t id);

/* File I/O */
void save_faculty(FacultyNode *list, const char *filename);
void load_faculty(FacultyNode **list, const char *filename);

/* Login */
Faculty *faculty_login(FacultyNode *list);

#endif
