#include "faculty_menu.h"

void faculty_menu(Faculty *faculty, FacultyNode **faculties, StudentNode **students)
{
  int choice = -1;
  while (choice != 0)
  {
    // Show actions:
    printf(
        "======= Faculty dashboard =======\n"
        "1. View Assigned Courses\n"
        "2. View Advisees\n"
        "3. Add Advisee Course\n"
        "4. Drop Advisee Course\n"
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
      for (FacultyCourseNode *cur = faculty->courses; cur; cur = cur->next)
      {
        printf("%s Section(%d)\n", cur->code, cur->section);
      }
      break;
    }
    case 2:
    {
      for (StudentNode *cur = *students; cur; cur = cur->next)
      {
        if (cur->student->advisor->id == faculty->id)
        {
          print_student(cur->student);
        }
      }
      break;
    }
    case 3:
    {
      uint32_t student_id;
      char course_code[MAX_COURSE_CODE_LENGTH];
      uint8_t section;

      printf("Enter student ID: ");
      if (scanf("%u", &student_id) != 1)
      {
        while (getchar() != '\n')
          ;
        break;
      }

      getchar();

      Student *student = find_student_by_id(students, student_id);
      if (!student)
      {
        printf("Student not found!\n");
        break;
      }

      if (student->advisor->id != faculty->id)
      {
        printf("You are not the advisor of this student!\n");
        break;
      }

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
      uint32_t student_id;
      char course_code[MAX_COURSE_CODE_LENGTH];
      uint8_t section;

      printf("Enter student ID: ");
      if (scanf("%u", &student_id) != 1)
      {
        while (getchar() != '\n')
          ;
        break;
      }

      getchar();

      Student *student = find_student_by_id(students, student_id);
      if (!student)
      {
        printf("Student not found!\n");
        break;
      }

      if (student->advisor->id != faculty->id)
      {
        printf("You are not the advisor of this student!\n");
        break;
      }

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

    save_faculty(*faculties, FACULTY_FILE);
    save_students(*students, STUDENTS_FILE);
  }
}