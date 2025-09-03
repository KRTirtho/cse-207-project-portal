#ifndef ADMIN_MENU_H
#define ADMIN_MENU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "admin.h"
#include "faculty.h"
#include "student.h"

#define ADMINS_FILE "admins.json"
#define FACULTY_FILE "faculty.json"
#define STUDENTS_FILE "students.json"


void admin_menu(AdminNode **admins, FacultyNode **faculties, StudentNode **students);
void create_root_admin(AdminNode **admins);
void save_all(AdminNode *a, FacultyNode *f, StudentNode *s);

#endif