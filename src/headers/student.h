#ifndef STUDENT_H
#define STUDENT_H

#include <stdint.h>
#include <stdio.h>
#include "faculty.h"

typedef struct StudentCourseNode {
    uint16_t courseID;
    struct StudentCourseNode *next;
} StudentCourseNode;

typedef struct Student {
    uint32_t id;
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char department[MAX_DEPARTMENT_LENGTH];
    uint16_t admission_year;
    uint8_t admission_month;
    float cgpa;
    Faculty *advisor;
    StudentCourseNode *courses;
    struct Student *next;
} Student;

typedef struct StudentNode {
    Student *student;
    struct StudentNode *next;
} StudentNode;

/* Student CRUD */
Student *create_student(StudentNode **list, uint32_t id, const char *name, const char *email, const char *password,
                        const char *dept, uint16_t year, uint8_t month, Faculty *advisor);
int delete_student(StudentNode **list, uint32_t id);
void print_student_list(StudentNode *list);

/* File I/O
   - load_students needs faculty list to relink advisor pointer by advisor_id saved in file. */
void save_students(StudentNode *list, const char *filename);
void load_students(StudentNode **list, const char *filename, FacultyNode *faculty_list);

#endif
