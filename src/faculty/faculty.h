#ifndef FACULTY_H
#define FACULTY_H

#include <stdint.h>
#include <stdio.h>
#include "../admin/admin.h"

typedef struct Faculty {
    uint32_t id;
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char department[MAX_DEPARTMENT_LENGTH];
    struct Faculty *next;
} Faculty;

typedef struct FacultyNode {
    Faculty *faculty;
    struct FacultyNode *next;
} FacultyNode;

/* Faculty CRUD */
Faculty *create_faculty(FacultyNode **list, uint32_t id, const char *name, const char *email, const char *password, const char *dept);
int delete_faculty(FacultyNode **list, uint32_t id);
void print_faculty_list(FacultyNode *list);

/* Helpers */
Faculty *find_faculty_by_id(FacultyNode *list, uint32_t id);

/* File I/O */
void save_faculty(FacultyNode *list, const char *filename);
void load_faculty(FacultyNode **list, const char *filename);

#endif
