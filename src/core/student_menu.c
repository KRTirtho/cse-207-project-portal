#include "student_menu.h"

void student_menu(Student *student, StudentNode **students, FacultyNode **faculty)
{
  int choice = -1;
  while (choice != 0)
  {
    // Show actions:
    printf(
        "======= Student dashboard =======\n"
        "1. View Courses\n"
        "2. View Advisor\n"
        "3. Add Course\n"
        "4. Drop Course\n"
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
      for (StudentCourseNode *cur = student->courses; cur; cur = cur->next)
      {
        printf("%s Section(%d)\n", cur->code, cur->section);
      }
      break;
    }
    case 2:
    {
      print_faculty(student->advisor);
      break;
    }
    case 3:
    {
      char course_code[MAX_COURSE_CODE_LENGTH];
      uint8_t section;

      printf("Course Code: ");
      fgets(course_code, sizeof(course_code), stdin);
      course_code[strcspn(course_code, "\n")] = 0;

      printf("Section: ");
      if (scanf("%hhu", &section) != 1)
      {
        while (getchar() != '\n')
          ;
        break;
      }

      push_student_course(student, course_code, section);
      break;
    }
    case 4:
    {
      char course_code[MAX_COURSE_CODE_LENGTH];
      uint8_t section;

      printf("Course Code: ");
      fgets(course_code, sizeof(course_code), stdin);
      course_code[strcspn(course_code, "\n")] = 0;

      printf("Section: ");
      if (scanf("%hhu", &section) != 1)
      {
        while (getchar() != '\n')
          ;
        break;
      }

      remove_student_course(student, course_code, section);
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

    save_students(*students, STUDENTS_FILE);
  }
}