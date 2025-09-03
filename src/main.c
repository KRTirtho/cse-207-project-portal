#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "admin.h"
#include "admin_menu.h"
#include "faculty.h"
#include "faculty_menu.h"
#include "student.h"
#include "student_menu.h"



static void main_menu(void);

int main(void)
{
    AdminNode *admins = NULL;
    FacultyNode *faculties = NULL;
    StudentNode *students = NULL;
    load_admins(&admins, ADMINS_FILE);
    load_faculty(&faculties, FACULTY_FILE);
    load_students(&students, STUDENTS_FILE, faculties);

    int choice = -1;
    while (choice != 0)
    {
        main_menu();
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ;
            continue;
        }
        getchar();
        switch (choice)
        {
        case 1:
        {
            Admin *logged = admin_login(admins);
            if (logged)
                admin_menu(&admins, &faculties, &students);
            else
                printf("Invalid credentials.\n");
            save_all(admins, faculties, students);
        }
        break;
        case 2:
        {
            Faculty *logged = faculty_login(faculties);
            if (logged)
                faculty_menu(logged, &faculties, &students);
            else
                printf("Invalid credentials.\n");
            save_all(admins, faculties, students);
        }
        break;
        case 3:
        {
            Student *logged = student_login(students);
            if (logged)
                student_menu(logged, &students, &faculties);
            else
                printf("Invalid credentials.\n");
            save_all(admins, faculties, students);
        }
        break;
        case 4:
        {
            save_all(admins, faculties, students);
            printf("Saved to files.\n");
            break;
        }
        case 5:
        {
            create_root_admin(&admins);
            save_all(admins, faculties, students);
            break;
        }
        case 0:
        {
            printf("Exiting...\n");
            break;
        }
        default:
            printf("Invalid option!\n");
        }
    }
    save_all(admins, faculties, students);
    return 0;
}

static void main_menu(void)
{
    printf(
        "======== University Portal ========\n"
        "1. Admin Login\n"
        "2. Faculty Login\n"
        "3. Student Login\n"
        "4. Save All\n"
        "5. Create admin\n"
        "0. Exit\n"
        "Select: ");
}