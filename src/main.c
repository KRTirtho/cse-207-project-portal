#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "admin.h"
#include "faculty.h"
#include "student.h"

#define ADMINS_FILE "admins.txt"
#define FACULTY_FILE "faculty.txt"
#define STUDENTS_FILE "students.txt"

static void main_menu(void);
static void save_all(AdminNode *a, FacultyNode *f, StudentNode *s);

static void admin_menu(AdminNode **admins, FacultyNode **faculties, StudentNode **students);
static void faculty_menu(FacultyNode **faculty);
static void student_menu(StudentNode **students, FacultyNode **faculty);

static void create_root_admin(AdminNode **admins);

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
                faculty_menu(&faculties);
            else
                printf("Invalid credentials.\n");
            save_all(admins, faculties, students);
        }
        break;
        case 3:
        {
            Student *logged = student_login(students);
            if (logged)
                student_menu(&students, &faculties);
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

static void save_all(AdminNode *a, FacultyNode *f, StudentNode *s)
{
    save_admins(a, ADMINS_FILE);
    save_faculty(f, FACULTY_FILE);
    save_students(s, STUDENTS_FILE);
}

void admin_menu(AdminNode **admins, FacultyNode **faculties, StudentNode **students)
{
    int choice = -1;
    while (choice != 0)
    {
        // Show actions:
        printf(
            "======= Admin dashboard =======\n"
            "1. View All Admins\n"
            "2. View All Faculty\n"
            "3. View All Students\n"
            "4. Add Admin\n"
            "5. Remove Admin\n"
            "6. Add Faculty\n"
            "7. Remove Faculty\n"
            "8. Add Student\n"
            "9. Remove Student\n"
            "0. Logout\n"
            "Select: ");

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
            printf("All Admins:\n");
            print_admin_list(*admins);
            break;
        }
        case 2:
        {
            printf("All Faculty:\n");
            print_faculty_list(*faculties);
            break;
        }
        case 3:
        {
            printf("All Students:\n");
            print_student_list(*students);
            break;
        }
        case 4:
        {
            create_root_admin(admins);
            break;
        }
        case 5:
        {
            uint32_t admin_del_id;
            printf("Admin ID to remove: ");
            if (scanf("%u", &admin_del_id) != 1)
            {
                while (getchar() != '\n')
                    ;
                break;
            }
            getchar();
            delete_admin(admins, admin_del_id);
            break;
        }
        case 6:
        {
            uint32_t faculty_add_id;
            char name[MAX_NAME_LENGTH], email[MAX_EMAIL_LENGTH], pass[MAX_PASSWORD_LENGTH];

            printf("Faculty ID: ");
            if (scanf("%u", &faculty_add_id) != 1)
            {
                while (getchar() != '\n')
                    ;
                break;
            }

            getchar();

            printf("Name: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;

            printf("Email: ");
            fgets(email, sizeof(email), stdin);
            email[strcspn(email, "\n")] = 0;

            printf("Password: ");
            fgets(pass, sizeof(pass), stdin);
            pass[strcspn(pass, "\n")] = 0;

            printf("Department: ");
            char dept[MAX_DEPARTMENT_LENGTH];
            fgets(dept, sizeof(dept), stdin);
            dept[strcspn(dept, "\n")] = 0;

            create_faculty(faculties, faculty_add_id, name, email, pass, dept);
            break;
        }
        case 7:
        {
            uint32_t faculty_delete_id;
            printf("Faculty ID to remove: ");
            if (scanf("%u", &faculty_delete_id) != 1)
            {
                while (getchar() != '\n')
                    ;
                break;
            }
            getchar();
            delete_faculty(faculties, faculty_delete_id);
            break;
        }
        case 8:
        {
            uint32_t advisor_id, student_add_id;
            char name[MAX_NAME_LENGTH], email[MAX_EMAIL_LENGTH], password[MAX_PASSWORD_LENGTH], dept[MAX_DEPARTMENT_LENGTH];
            uint16_t year;
            uint8_t month;
            Faculty *advisor = NULL;

            printf("Student ID: ");
            if (scanf("%u", &student_add_id) != 1)
            {
                while (getchar() != '\n')
                    ;
                break;
            }
            getchar();

            printf("Name: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;

            printf("Email: ");
            fgets(email, sizeof(email), stdin);
            email[strcspn(email, "\n")] = 0;

            printf("Password: ");
            fgets(password, sizeof(password), stdin);
            password[strcspn(password, "\n")] = 0;

            printf("Department: ");
            fgets(dept, sizeof(dept), stdin);
            dept[strcspn(dept, "\n")] = 0;

            printf("Year: ");
            if (scanf("%hu", &year) != 1)
            {
                while (getchar() != '\n')
                    ;
                break;
            }

            printf("Month: ");
            if (scanf("%hhu", &month) != 1)
            {
                while (getchar() != '\n')
                    ;
                break;
            }

            printf("Advisor Faculty ID: ");
            if (scanf("%u", &advisor_id) != 1)
            {
                while (getchar() != '\n')
                    ;
                break;
            }
            getchar();

            advisor = find_faculty_by_id(*faculties, advisor_id);
            if (!advisor)
            {
                printf("Faculty not found to assign as advisor!\n");
                break;
            }

            create_student(students, student_add_id, name, email, password, dept, year, month, advisor);
            break;
        }
        case 9:
        {
            uint32_t student_delete_id;
            printf("Student ID to remove: ");
            if (scanf("%u", &student_delete_id) != 1)
            {
                while (getchar() != '\n')
                    ;
                break;
            }
            getchar();
            delete_student(students, student_delete_id);
            break;
        }
        case 0:
        {
            printf("Logging out...\n");
            break;
        }
        default:
            printf("Invalid option!\n");
        }

        save_all(*admins, *faculties, *students);
    }
}

void faculty_menu(FacultyNode **faculty)
{
}

void student_menu(StudentNode **students, FacultyNode **faculty)
{
}

void create_root_admin(AdminNode **admins)
{
    uint32_t id;
    char name[MAX_NAME_LENGTH], email[MAX_EMAIL_LENGTH], pass[MAX_PASSWORD_LENGTH];

    printf("ID: ");
    if (scanf("%u", &id) != 1)
    {
        while (getchar() != '\n')
            ;
        return;
    }

    getchar(); // Clear newline character from input buffer

    printf("\nName: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    printf("\nEmail: ");
    fgets(email, sizeof(email), stdin);
    email[strcspn(email, "\n")] = 0;

    printf("Password: ");
    fgets(pass, sizeof(pass), stdin);
    pass[strcspn(pass, "\n")] = 0;

    create_admin(admins, id, name, email, pass);
}
