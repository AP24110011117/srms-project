#pragma comment(linker, "/SUBSYSTEM:CONSOLE")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>

#define STUDENT_FILE    "students.txt"
#define CREDENTIAL_FILE "credentials.txt"
#define MAX_STUDENTS    200
#define MAX_USERS       200

struct student {
    char roll[21];
    char name[100];
    float marks;
};

struct credential {
    char username[50];
    char password[50];
    char role[20];
};

char currentRole[20];
char currentUser[50];

void getMaskedPassword(char *password, int maxLen) {
    int i = 0;
    char ch;
    while (1) {
        ch = getch();
        if (ch == 13) break;
        if (ch == 8 && i > 0) {
            i--;
            printf("\b \b");
            continue;
        }
        if (i < maxLen - 1 && ch >= 32 && ch <= 126) {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");
}

int loginSystem() {
    char username[50], password[50], u[50], p[50], r[20];
    printf("==== LOGIN ====\n");
    printf("Username: ");
    scanf("%49s", username);
    printf("Password: ");
    getMaskedPassword(password, 50);
    FILE *fp = fopen(CREDENTIAL_FILE, "r");
    if (!fp) return 0;
    while (fscanf(fp, "%49s %49s %19s", u, p, r) == 3) {
        if (strcmp(username, u) == 0 && strcmp(password, p) == 0) {
            strcpy(currentRole, r);
            strcpy(currentUser, u);
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void clearInputLine() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void getRoll(char *roll) {
    while (1) {
        scanf("%20s", roll);
        int valid = 1;
        for (int i = 0; roll[i]; i++) {
            if (!isalnum((unsigned char)roll[i])) {
                valid = 0;
                break;
            }
        }
        if (valid) return;
        printf("Invalid roll! Only alphabets and numbers allowed: ");
    }
}

float getFloat() {
    float x;
    while (scanf("%f", &x) != 1 || x < 0) {
        printf("Invalid marks, enter again: ");
        clearInputLine();
    }
    return x;
}

void getName(char *name) {
    while (1) {
        if (!fgets(name, 100, stdin)) {
            name[0] = '\0';
            return;
        }
        if (name[0] == '\n') {
            printf("Name cannot be empty: ");
            continue;
        }
        name[strcspn(name, "\n")] = '\0';
        int valid = 1;
        for (int i = 0; name[i]; i++) {
            if (!(isalpha((unsigned char)name[i]) || name[i] == ' ')) {
                valid = 0;
                break;
            }
        }
        if (valid) return;
        printf("Invalid name, alphabets and spaces only: ");
    }
}

void addStudent() {
    struct student s;
    printf("Enter Roll: ");
    getRoll(s.roll);
    clearInputLine();
    printf("Enter Name: ");
    getName(s.name);
    printf("Enter Marks: ");
    s.marks = getFloat();
    FILE *fp = fopen(STUDENT_FILE, "a");
    if (!fp) {
        printf("Error opening student file\n");
        return;
    }
    fprintf(fp, "%s|%s|%.2f\n", s.roll, s.name, s.marks);
    fclose(fp);
    printf("Student Added\n");
}

int readStudentRecord(FILE *fp, struct student *s) {
    return fscanf(fp, "%20[^|]|%99[^|]|%f\n", s->roll, s->name, &s->marks) == 3;
}

void displayStudents() {
    struct student s;
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) {
        printf("No Records\n");
        return;
    }
    printf("\n----------------------------------------------------------------------\n");
    printf("%-20s %-40s %-10s\n", "ROLL", "NAME", "MARKS");
    printf("----------------------------------------------------------------------\n");
    while (readStudentRecord(fp, &s)) {
        printf("%-20s %-40s %-10.2f\n", s.roll, s.name, s.marks);
    }
    printf("----------------------------------------------------------------------\n");
    fclose(fp);
}

void searchStudent() {
    char roll[21];
    struct student s;
    int found = 0;
    printf("Enter Roll: ");
    getRoll(roll);
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) {
        printf("No Records\n");
        return;
    }
    while (readStudentRecord(fp, &s)) {
        if (strcmp(s.roll, roll) == 0) {
            printf("\nRoll : %s\nName : %s\nMarks: %.2f\n", s.roll, s.name, s.marks);
            found = 1;
            break;
        }
    }
    if (!found) printf("Not Found\n");
    fclose(fp);
}

void updateMarksOnly() {
    char roll[21];
    struct student s;
    int found = 0;

    printf("Enter Roll: ");
    getRoll(roll);

    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) {
        printf("No Records\n");
        return;
    }
    FILE *tmp = fopen("temp.txt", "w");
    if (!tmp) {
        fclose(fp);
        printf("Error opening temp file\n");
        return;
    }

    while (readStudentRecord(fp, &s)) {
        if (strcmp(s.roll, roll) == 0) {
            found = 1;
            printf("Current Marks: %.2f\n", s.marks);
            printf("Enter New Marks: ");
            s.marks = getFloat();
        }
        fprintf(tmp, "%s|%s|%.2f\n", s.roll, s.name, s.marks);
    }

    fclose(fp);
    fclose(tmp);
    remove(STUDENT_FILE);
    rename("temp.txt", STUDENT_FILE);

    printf(found ? "Marks Updated\n" : "Not Found\n");
}


void updateStudent() {
    char roll[21];
    struct student s;
    int found = 0;
    printf("Enter Roll: ");
    getRoll(roll);
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) {
        printf("No Records\n");
        return;
    }
    FILE *tmp = fopen("temp.txt", "w");
    if (!tmp) {
        fclose(fp);
        printf("Error opening temp file\n");
        return;
    }
    while (readStudentRecord(fp, &s)) {
        if (strcmp(s.roll, roll) == 0) {
            found = 1;
            clearInputLine();
            printf("New Name: ");
            getName(s.name);
            printf("New Marks: ");
            s.marks = getFloat();
        }
        fprintf(tmp, "%s|%s|%.2f\n", s.roll, s.name, s.marks);
    }
    fclose(fp);
    fclose(tmp);
    remove(STUDENT_FILE);
    rename("temp.txt", STUDENT_FILE);
    printf(found ? "Updated\n" : "Not Found\n");
}

void deleteStudent() {
    char roll[21];
    struct student s;
    int found = 0;
    printf("Enter Roll: ");
    getRoll(roll);
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) {
        printf("No Records\n");
        return;
    }
    FILE *tmp = fopen("temp.txt", "w");
    if (!tmp) {
        fclose(fp);
        printf("Error opening temp file\n");
        return;
    }
    while (readStudentRecord(fp, &s)) {
        if (strcmp(s.roll, roll) == 0) {
            found = 1;
            continue;
        }
        fprintf(tmp, "%s|%s|%.2f\n", s.roll, s.name, s.marks);
    }
    fclose(fp);
    fclose(tmp);
    remove(STUDENT_FILE);
    rename("temp.txt", STUDENT_FILE);
    printf(found ? "Deleted\n" : "Not Found\n");
}

void loadStudents(struct student arr[], int *count) {
    int i = 0;
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) {
        *count = 0;
        return;
    }
    while (i < MAX_STUDENTS && readStudentRecord(fp, &arr[i])) {
        i++;
    }
    *count = i;
    fclose(fp);
}

void displayStudentsFromArray(struct student arr[], int count) {
    printf("\n----------------------------------------------------------------------\n");
    printf("%-20s %-40s %-10s\n", "ROLL", "NAME", "MARKS");
    printf("----------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++)
        printf("%-20s %-40s %-10.2f\n", arr[i].roll, arr[i].name, arr[i].marks);
    printf("----------------------------------------------------------------------\n");
}

void sortByRoll(struct student arr[], int count) {
    struct student t;
    for (int i = 0; i < count - 1; i++)
        for (int j = 0; j < count - 1 - i; j++)
            if (strcmp(arr[j].roll, arr[j + 1].roll) > 0) {
                t = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = t;
            }
}

void sortByName(struct student arr[], int count) {
    struct student t;
    for (int i = 0; i < count - 1; i++)
        for (int j = 0; j < count - 1 - i; j++)
            if (strcmp(arr[j].name, arr[j + 1].name) > 0) {
                t = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = t;
            }
}

void sortByMarksAsc(struct student arr[], int count) {
    struct student t;
    for (int i = 0; i < count - 1; i++)
        for (int j = 0; j < count - 1 - i; j++)
            if (arr[j].marks > arr[j + 1].marks) {
                t = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = t;
            }
}

void sortByMarksDesc(struct student arr[], int count) {
    struct student t;
    for (int i = 0; i < count - 1; i++)
        for (int j = 0; j < count - 1 - i; j++)
            if (arr[j].marks < arr[j + 1].marks) {
                t = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = t;
            }
}

void sortStudentsMenu() {
    struct student arr[MAX_STUDENTS];
    int count, c;
    loadStudents(arr, &count);
    if (count == 0) {
        printf("No Records\n");
        return;
    }
    printf("\n1.Roll\n2.Name\n3.Marks Asc\n4.Marks Desc\nChoice: ");
    scanf("%d", &c);
    switch (c) {
        case 1: sortByRoll(arr, count); break;
        case 2: sortByName(arr, count); break;
        case 3: sortByMarksAsc(arr, count); break;
        case 4: sortByMarksDesc(arr, count); break;
        default: return;
    }
    displayStudentsFromArray(arr, count);
}

int loadCredentials(struct credential users[], int *count) {
    int i = 0;
    FILE *fp = fopen(CREDENTIAL_FILE, "r");
    if (!fp) {
        *count = 0;
        return 0;
    }
    while (i < MAX_USERS &&
           fscanf(fp, "%49s %49s %19s", users[i].username, users[i].password, users[i].role) == 3)
        i++;
    *count = i;
    fclose(fp);
    return 1;
}

void listUsers() {
    struct credential u[MAX_USERS];
    int c;
    loadCredentials(u, &c);
    printf("\n-----------------------------------------------\n");
    printf("%-20s %-20s %-10s\n", "USERNAME", "PASSWORD", "ROLE");
    printf("-----------------------------------------------\n");
    for (int i = 0; i < c; i++)
        printf("%-20s %-20s %-10s\n", u[i].username, u[i].password, u[i].role);
    printf("-----------------------------------------------\n");
}

void addUser() {
    char u[50], p[50], r[20];
    printf("Username: ");
    scanf("%49s", u);
    printf("Password: ");
    scanf("%49s", p);
    printf("Role: ");
    scanf("%19s", r);
    FILE *fp = fopen(CREDENTIAL_FILE, "a");
    if (!fp) {
        printf("Error opening credential file\n");
        return;
    }
    fprintf(fp, "%s %s %s\n", u, p, r);
    fclose(fp);
    printf("Added\n");
}

void deleteUser() {
    char udel[50];
    struct credential u[MAX_USERS];
    int count, found = 0;
    printf("Username: ");
    scanf("%49s", udel);
    loadCredentials(u, &count);
    FILE *fp = fopen(CREDENTIAL_FILE, "w");
    if (!fp) {
        printf("Error opening credential file\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        if (strcmp(u[i].username, udel) == 0) {
            found = 1;
            continue;
        }
        fprintf(fp, "%s %s %s\n", u[i].username, u[i].password, u[i].role);
    }
    fclose(fp);
    printf(found ? "Deleted\n" : "Not Found\n");
}

void changeUserPassword() {
    char uchg[50], np[50];
    struct credential u[MAX_USERS];
    int count, found = 0;
    printf("Username: ");
    scanf("%49s", uchg);
    printf("New Password: ");
    scanf("%49s", np);
    loadCredentials(u, &count);
    for (int i = 0; i < count; i++) {
        if (strcmp(u[i].username, uchg) == 0) {
            strcpy(u[i].password, np);
            found = 1;
        }
    }
    FILE *fp = fopen(CREDENTIAL_FILE, "w");
    if (!fp) {
        printf("Error opening credential file\n");
        return;
    }
    for (int i = 0; i < count; i++)
        fprintf(fp, "%s %s %s\n", u[i].username, u[i].password, u[i].role);
    fclose(fp);
    printf(found ? "Updated\n" : "Not Found\n");
}

void manageUsersMenu() {
    int c;
    while (1) {
        printf("\n1.List\n2.Add\n3.Delete\n4.Change Pass\n5.Back\nChoice: ");
        scanf("%d", &c);
        switch (c) {
            case 1: listUsers(); break;
            case 2: addUser(); break;
            case 3: deleteUser(); break;
            case 4: changeUserPassword(); break;
            case 5: return;
        }
    }
}

void adminMenu();
void userMenu();
void staffMenu();
void guestMenu();

void mainMenu() {
    if (strcmp(currentRole, "ADMIN") == 0) adminMenu();
    else if (strcmp(currentRole, "USER") == 0) userMenu();
    else if (strcmp(currentRole, "STAFF") == 0) staffMenu();
    else guestMenu();
}

void adminMenu() {
    int c;
    while (1) {
        printf("\n1.Add\n2.Display\n3.Search\n4.Update\n5.Delete\n6.Sort\n7.Users\n8.Logout\nChoice: ");
        scanf("%d", &c);
        switch (c) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
            case 5: deleteStudent(); break;
            case 6: sortStudentsMenu(); break;
            case 7: manageUsersMenu(); break;
            case 8: return;
        }
    }
}

void userMenu() {
    int c;
    while (1) {
        printf("\n1.Display\n2.Search\n3.Sort\n4.Logout\nChoice: ");
        scanf("%d", &c);
        switch (c) {
            case 1: displayStudents(); break;
            case 2: searchStudent(); break;
            case 3: sortStudentsMenu(); break;
            case 4: return;
        }
    }
}

void staffMenu() {
    int c;
    while (1) {
        printf("\n1.Display\n2.Search\n3.Update Marks Only\n4.Sort\n5.Logout\nChoice: ");
        scanf("%d", &c);
        switch (c) {
            case 1: displayStudents(); break;
            case 2: searchStudent(); break;
            case 3: updateMarksOnly(); break;
            case 4: sortStudentsMenu(); break;
            case 5: return;
        }
    }
}

void guestMenu() {
    printf("\nGuests can view only.\n");
    displayStudents();
}

int main() {
    if (loginSystem()) mainMenu();
    else printf("Access Denied\n");
    return 0;
}
