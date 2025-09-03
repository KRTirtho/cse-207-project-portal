#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./admin/admin.h"
#include "./faculty/faculty.h"
#include "./student/student.h"

#define ADMINS_FILE "admins.txt"
#define FACULTY_FILE "faculty.txt"
#define STUDENTS_FILE "students.txt"

static void mainMenu(void);
static void adminMenu(AdminNode **admins);
static void facultyMenu(FacultyNode **faculty);
static void studentMenu(StudentNode **students, FacultyNode *facultyList);

/* optional: write immediately after each change */
static void save_all(AdminNode *a, FacultyNode *f, StudentNode *s)
{
    save_admins(a, ADMINS_FILE);
    save_faculty(f, FACULTY_FILE);
    save_students(s, STUDENTS_FILE);
}

int main(void)
{
    AdminNode *admins = NULL;
    FacultyNode *facultyList = NULL;
    StudentNode *students = NULL;

    /* Load persisted data on start */
    load_admins(&admins, ADMINS_FILE);
    load_faculty(&facultyList, FACULTY_FILE);
    load_students(&students, STUDENTS_FILE, facultyList);

    int choice = -1;
    while (choice != 0)
    {
        mainMenu();
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ;
            continue;
        }
        getchar(); /* consume newline */

        switch (choice)
        {
        case 1:
            adminMenu(&admins);
            save_all(admins, facultyList, students);
            break;
        case 2:
            facultyMenu(&facultyList);
            save_all(admins, facultyList, students);
            break;
        case 3:
            studentMenu(&students, facultyList);
            save_all(admins, facultyList, students);
            break;
        case 4:
            printf("All Admins:\n");
            print_admin_list(admins);
            break;
        case 5:
            printf("All Faculty:\n");
            print_faculty_list(facultyList);
            break;
        case 6:
            printf("All Students:\n");
            print_student_list(students);
            break;
        case 7:
            save_all(admins, facultyList, students);
            printf("Saved to files.\n");
            break;
        case 0:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid option!\n");
        }
    }

    /* Save on exit (defensive) */
    save_all(admins, facultyList, students);
    return 0;
}

static void mainMenu(void)
{
    printf("\n==== University Management ====\n");
    printf("1. Admin\n2. Faculty\n3. Student\n4. View Admins\n5. View Faculty\n6. View Students\n7. Save Now\n0. Exit\nSelect: ");
}

static void adminMenu(AdminNode **admins)
{
    int opt;
    printf("Admin Menu: 1-Create 2-Delete 3-View: ");
    if (scanf("%d", &opt) != 1)
    {
        while (getchar() != '\n')
            ;
        return;
    }
    getchar();
    if (opt == 1)
    {
        uint32_t id;
        char name[MAX_NAME_LENGTH], email[MAX_EMAIL_LENGTH], password[MAX_PASSWORD_LENGTH];
        printf("ID: ");
        if (scanf("%u", &id) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        getchar();
        printf("Name: ");
        fgets(name, MAX_NAME_LENGTH, stdin);
        name[strcspn(name, "\n")] = 0;
        printf("Email: ");
        fgets(email, MAX_EMAIL_LENGTH, stdin);
        email[strcspn(email, "\n")] = 0;
        printf("Password: ");
        fgets(password, MAX_PASSWORD_LENGTH, stdin);
        password[strcspn(password, "\n")] = 0;
        if (create_admin(admins, id, name, email, password))
            printf("Admin created.\n");
        else
            printf("Failed to create admin.\n");
    }
    else if (opt == 2)
    {
        uint32_t id;
        printf("ID to delete: ");
        if (scanf("%u", &id) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        getchar();
        if (delete_admin(admins, id) == 0)
            printf("Deleted.\n");
        else
            printf("Not found.\n");
    }
    else if (opt == 3)
    {
        print_admin_list(*admins);
    }
    else
        printf("Invalid.\n");
}

static void facultyMenu(FacultyNode **faculty)
{
    int opt;
    printf("Faculty Menu: 1-Create 2-Delete 3-View: ");
    if (scanf("%d", &opt) != 1)
    {
        while (getchar() != '\n')
            ;
        return;
    }
    getchar();
    if (opt == 1)
    {
        uint32_t id;
        char name[MAX_NAME_LENGTH], email[MAX_EMAIL_LENGTH], pass[MAX_PASSWORD_LENGTH], dept[MAX_DEPARTMENT_LENGTH];
        printf("ID: ");
        if (scanf("%u", &id) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        getchar();
        printf("Name: ");
        fgets(name, MAX_NAME_LENGTH, stdin);
        name[strcspn(name, "\n")] = 0;
        printf("Email: ");
        fgets(email, MAX_EMAIL_LENGTH, stdin);
        email[strcspn(email, "\n")] = 0;
        printf("Password: ");
        fgets(pass, MAX_PASSWORD_LENGTH, stdin);
        pass[strcspn(pass, "\n")] = 0;
        printf("Dept (e.g., CSE): ");
        fgets(dept, MAX_DEPARTMENT_LENGTH, stdin);
        dept[strcspn(dept, "\n")] = 0;
        if (create_faculty(faculty, id, name, email, pass, dept))
            printf("Faculty created.\n");
        else
            printf("Failed to create faculty.\n");
    }
    else if (opt == 2)
    {
        uint32_t id;
        printf("ID to delete: ");
        if (scanf("%u", &id) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        getchar();
        if (delete_faculty(faculty, id) == 0)
            printf("Deleted.\n");
        else
            printf("Not found.\n");
    }
    else if (opt == 3)
    {
        print_faculty_list(*faculty);
    }
    else
        printf("Invalid.\n");
}

static void studentMenu(StudentNode **students, FacultyNode *facultyList)
{
    int opt;
    printf("Student Menu: 1-Create 2-Delete 3-View 4-Assign/Change Advisor 5-Add CourseID 6-Remove CourseID: ");
    if (scanf("%d", &opt) != 1)
    {
        while (getchar() != '\n')
            ;
        return;
    }
    getchar();
    if (opt == 1)
    {
        uint32_t id;
        char name[MAX_NAME_LENGTH], email[MAX_EMAIL_LENGTH], pass[MAX_PASSWORD_LENGTH], dept[MAX_DEPARTMENT_LENGTH];
        uint16_t year;
        uint8_t month;
        uint32_t adv_id = 0;
        printf("ID: ");
        if (scanf("%u", &id) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        getchar();
        printf("Name: ");
        fgets(name, MAX_NAME_LENGTH, stdin);
        name[strcspn(name, "\n")] = 0;
        printf("Email: ");
        fgets(email, MAX_EMAIL_LENGTH, stdin);
        email[strcspn(email, "\n")] = 0;
        printf("Password: ");
        fgets(pass, MAX_PASSWORD_LENGTH, stdin);
        pass[strcspn(pass, "\n")] = 0;
        printf("Dept (e.g., CSE): ");
        fgets(dept, MAX_DEPARTMENT_LENGTH, stdin);
        dept[strcspn(dept, "\n")] = 0;
        printf("Admission Year: ");
        if (scanf("%hu", &year) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        printf("Admission Month: ");
        if (scanf("%hhu", &month) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        printf("Advisor Faculty ID (0 for none): ");
        if (scanf("%u", &adv_id) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        getchar();
        Faculty *adv = adv_id ? find_faculty_by_id(facultyList, adv_id) : NULL;
        if (create_student(students, id, name, email, pass, dept, year, month, adv))
            printf("Student created.\n");
        else
            printf("Failed to create student.\n");
    }
    else if (opt == 2)
    {
        uint32_t id;
        printf("ID to delete: ");
        if (scanf("%u", &id) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        getchar();
        if (delete_student(students, id) == 0)
            printf("Deleted.\n");
        else
            printf("Not found.\n");
    }
    else if (opt == 3)
    {
        print_student_list(*students);
    }
    else if (opt == 4)
    {
        uint32_t sid, fid;
        printf("Student ID: ");
        if (scanf("%u", &sid) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        printf("New Faculty ID (0 to clear): ");
        if (scanf("%u", &fid) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        getchar();
        StudentNode *s = *students;
        Student *target = NULL;
        while (s)
        {
            if (s->student->id == sid)
            {
                target = s->student;
                break;
            }
            s = s->next;
        }
        if (!target)
        {
            printf("Student not found.\n");
            return;
        }
        target->advisor = fid ? find_faculty_by_id(facultyList, fid) : NULL;
        printf("Advisor %s.\n", target->advisor ? "updated" : "cleared");
    }
    else if (opt == 5)
    {
        uint32_t sid;
        uint16_t cid;
        printf("Student ID: ");
        if (scanf("%u", &sid) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        printf("Course ID to add: ");
        if (scanf("%hu", &cid) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        getchar();
        StudentNode *s = *students;
        Student *target = NULL;
        while (s)
        {
            if (s->student->id == sid)
            {
                target = s->student;
                break;
            }
            s = s->next;
        }
        if (!target)
        {
            printf("Student not found.\n");
            return;
        }
        StudentCourseNode *n = (StudentCourseNode *)malloc(sizeof(StudentCourseNode));
        if (!n)
        {
            printf("Alloc fail.\n");
            return;
        }
        n->courseID = cid;
        n->next = NULL;
        if (!target->courses)
            target->courses = n;
        else
        {
            StudentCourseNode *c = target->courses;
            while (c->next)
                c = c->next;
            c->next = n;
        }
        printf("Course added.\n");
    }
    else if (opt == 6)
    {
        uint32_t sid;
        uint16_t cid;
        printf("Student ID: ");
        if (scanf("%u", &sid) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        printf("Course ID to remove: ");
        if (scanf("%hu", &cid) != 1)
        {
            while (getchar() != '\n')
                ;
            return;
        }
        getchar();
        StudentNode *s = *students;
        Student *target = NULL;
        while (s)
        {
            if (s->student->id == sid)
            {
                target = s->student;
                break;
            }
            s = s->next;
        }
        if (!target)
        {
            printf("Student not found.\n");
            return;
        }
        StudentCourseNode *c = target->courses, *p = NULL;
        while (c)
        {
            if (c->courseID == cid)
            {
                if (p)
                    p->next = c->next;
                else
                    target->courses = c->next;
                free(c);
                printf("Course removed.\n");
                return;
            }
            p = c;
            c = c->next;
        }
        printf("Course not found.\n");
    }
    else
    {
        printf("Invalid.\n");
    }
}
