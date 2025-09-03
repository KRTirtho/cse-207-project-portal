#ifndef STUDENT_MENU_H
#define STUDENT_MENU_H

#include <stdio.h>
#include <stdlib.h>

#include "admin_menu.h"
#include "faculty.h"
#include "student.h"

void student_menu(Student *student, StudentNode **students, FacultyNode **faculty);

#endif