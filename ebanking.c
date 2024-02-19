#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "include/colors.h"
#include <stdbool.h>
#include <unistd.h>
#include <conio.h>
#include <errno.h>

// Constants
#define MAX_USERNAME_LENGTH 32
#define MAX_PASSWORD_LENGTH 4
#define MAX_TRANSACTION_LOGS 100
#define MAX_USERS 1000
#define FILE_USER_DATA "user.txt"
#define FILE_DATABASE "database.txt"
#define FILE_TRANSACTIONS "transactions.txt"

// Structs
struct User
{
    char username[MAX_USERNAME_LENGTH];
    int mpin;
};

struct TransactionLog
{
    char username[MAX_USERNAME_LENGTH];
    float amount;
    char timestamp[30];
};

struct User users[MAX_USERS];

// Function Declarations
void clearBuffer();
void waitForEnter();
int registerAccount(struct User users[], int *numUsers);
int findUserIndex(char username[], struct User users[], int numUsers);
void saveUserToFile(struct User user);
void createNewRecord(char username[]);
void displayUserInfo(char username[]);
void loadMoney(char *username);
void sendMoney(char username[], char rusername[]);
void logTransaction(char username[], float amount);
void displayMainMenu();
int login(struct User users[], int numUsers);
int initMenu();
bool isValidMPIN(int mpin);

void loadUserData(struct User users[], int *numUsers);
void deleteUser(struct User users[], char username[], int numUsers);

// Additional Function Declarations
int getMPIN();
bool isUsernameExist(char username[]);
bool isMpinCorrect(char username[], int mpin);
void viewTransactions(char username[]);
bool confirmDeletion();
int handleLoginFailure(struct User users[], int numUsers);
int innerMenu(char username[], int userIndex);

// Main Function
int main()
{
    if (initMenu() == -1)
    {
        return 0;
    }

    return 0;
}

// Function Definitions

// Utility Functions
void clearBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

bool isValidMPIN(int mpin)
{
    return (mpin >= 1000 && mpin <= 9999);
}

void waitForEnter()
{
    printf("Press any key to continue...");
    fflush(stdout);
    clearBuffer();
    getchar();
}

// User Management Functions
int findUserIndex(char username[], struct User users[], int numUsers)
{
    for (int i = 0; i < numUsers; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            return i;
        }
    }
    return -1;
}

void saveUserToFile(struct User user)
{
    FILE *file = fopen(FILE_USER_DATA, "a");

    if (file != NULL)
    {
        fprintf(file, "%s %d \n", user.username, user.mpin);
        fclose(file);
    }
}

void createNewRecord(char username[])
{
    FILE *file = fopen(FILE_DATABASE, "a");

    if (file != NULL)
    {
        fprintf(file, "%s 0.00\n", username);
        fclose(file);
    }
}

void displayUserInfo(char username[]) {
    FILE* file = fopen(FILE_DATABASE, "r");
    if (file == NULL) {
        printf(LIGHT_RED "\nUnable to access the database.\n" RESET);
        return;
    }

    bool found = false;
    char currentUsername[MAX_USERNAME_LENGTH];
    float balance;

    while (fscanf(file, "%s %f", currentUsername, &balance) != EOF) {
        if (strcmp(currentUsername, username) == 0) {
            printf(BLUE "\n\n\n\n\t\t\t\t\t\t\t ACCOUNT INFORMATION\n" RESET);
            printf("\n\n\t\t\t\t\t\t Username: %s", currentUsername);
            printf("\n\n\t\t\t\t\t\t Balance: Rs. %.2f\n\n\n", balance);
            found = true;
            break; 
        }
    }

    fclose(file);

    if (!found) {
        createNewRecord(username);
        printf(GREEN "\nNew user record created!\n" RESET);
        displayUserInfo(username);
    }
}

bool confirmDeletion()
{
    printf(LIGHT_RED "\n\nWARNING: Deleting your account is irreversible and will result in the loss of all your data.\n" RESET);
    printf(LIGHT_YELLOW "Are you sure you want to proceed? (Y/N): " RESET);

    char choice;
    scanf(" %c", &choice);

    return (choice == 'Y' || choice == 'y');
}

void deleteUser(struct User users[], char username[], int numUsers)
{
    FILE *file = fopen(FILE_USER_DATA, "w");

    if (file == NULL)
    {
        printf(LIGHT_RED "\nError: Unable to access user data.\n" RESET);
        return;
    }

    for (int i = 0; i < numUsers; i++)
    {
        if (strcmp(users[i].username, username) != 0)
        {
            fprintf(file, "%s %d\n", users[i].username, users[i].mpin);
        }
    }

    fclose(file);
    system("cls");
    initMenu();
}

void loadUserData(struct User users[], int *numUsers)
{
    FILE *file = fopen(FILE_USER_DATA, "r");

    if (file == NULL)
    {
        printf(LIGHT_RED "\nError: Unable to access user data.\n" RESET);
        return;
    }

    int count = 0;
    while (count < MAX_USERS && fscanf(file, "%s %d", users[count].username, &users[count].mpin) == 2)
    {
        count++;
    }

    *numUsers = count;
    fclose(file);
}


//------------------------------------------------------ VALIDATION FUNCTIONS ------------------------------------------------------
bool isUsernameExist(char username[])
{
    FILE *inputFile = fopen(FILE_DATABASE, "r");
    if (inputFile == NULL)
    {
        printf(LIGHT_RED "\nUnable to access the database.\n" RESET);
        return false;
    }

    char currentUsername[MAX_USERNAME_LENGTH];
    float balance;
    while (fscanf(inputFile, "%s %f", currentUsername, &balance) != EOF)
    {
        if (strcmp(currentUsername, username) == 0)
        {
            fclose(inputFile);
            return true;
        }
    }

    fclose(inputFile);
    return false;
}

bool isMpinCorrect(char username[], int mpin)
{
    FILE *inputFile = fopen(FILE_USER_DATA, "r");
    if (inputFile == NULL)
    {
        printf(LIGHT_RED "\nUnable to access the database.\n" RESET);
        return false;
    }

    char currentUsername[MAX_USERNAME_LENGTH];
    int currentMpin;
    while (fscanf(inputFile, "%s %d", currentUsername, &currentMpin) != EOF)
    {
        if (strcmp(currentUsername, username) == 0 && currentMpin == mpin)
        {
            fclose(inputFile);
            return true;
        }
    }

    fclose(inputFile);
    return false;
}

// --------------------------------------------------------Transaction Functions-----------------------------------------------------
void loadMoney(char *username)
{
    printf(BLUE "\nEnter the amount you want to deposit: " RESET);

    float amount;
    scanf("%f", &amount);

    FILE *inputFile = fopen(FILE_DATABASE, "r");
    FILE *tempFile = fopen("temp.txt", "w");

    if (inputFile == NULL || tempFile == NULL)
    {
        printf(LIGHT_RED "\nUnable to access the database.\n" RESET);
        return;
    }

    char currentUsername[MAX_USERNAME_LENGTH];
    float balance;
    int found = 0;

    while (fscanf(inputFile, "%s %f", currentUsername, &balance) != EOF)
    {
        if (strcmp(currentUsername, username) == 0)
        {
            balance += amount;
            found = 1;
        }

        fprintf(tempFile, "%s %.2f\n", currentUsername, balance);
    }

    fclose(inputFile);
    fclose(tempFile);

    remove(FILE_DATABASE);
    rename("temp.txt", FILE_DATABASE);
    logTransaction(username, amount);
    if (found)
    {
        printf(GREEN "\nDeposit successful. The amount has been added to your account.\n" RESET);
        usleep(800000);
        system("cls");
        displayUserInfo(username);
    }
    else
    {
        printf(LIGHT_RED "\nUser not found.\n" RESET);
    }
}

void sendMoney(char username[], char rusername[])
{
    if (!isUsernameExist(rusername))
    {
        printf(LIGHT_RED "\nReceiver's username not found. \nPlease make sure you entered the correct username.\n" RESET);

        usleep(800000);
        return;
    }

    printf(BLUE "\nEnter the amount to send: " RESET);
    float amount;
    scanf("%f", &amount);

    if (amount < 10)
    {
        printf(LIGHT_RED "\nAmount should be at least 10. \nPlease enter a valid amount.\n" RESET);

        usleep(800000);
        return;
    }

    int i = 0;
    int mpin;
    char ch;
    char password[MAX_PASSWORD_LENGTH];
    bool passwordEntered = false;

    printf(BLUE "\nEnter your MPIN: " RESET);
    // scanf("%d", &mpin);

    while (i < MAX_PASSWORD_LENGTH)
    {
        ch = getch();

        if (ch == 13)
        {
            break;
        }
        else if (ch == 8)
        {
            if (i > 0)
            {
                printf("\b \b");
                i--;
            }
        }
        else if (ch >= '0' && ch <= '9')
        {
            password[i] = ch;
            i++;
            printf("*");
            passwordEntered = true;
        }
    }

    password[i] = '\0';

    if (passwordEntered)
    {
        mpin = atoi(password);
    }
    else
    {
        printf(LIGHT_RED "\nInvalid MPIN.\n" RESET);
    }

    FILE *inputFile = fopen(FILE_DATABASE, "r");
    FILE *tempFile = fopen("temp.txt", "w");

    if (inputFile == NULL || tempFile == NULL)
    {
        printf(LIGHT_RED "\nUnable to access the database.\n" RESET);
        return;
    }

    char currentUsername[MAX_USERNAME_LENGTH];
    float balance;
    int found = 0;

    while (fscanf(inputFile, "%s %f", currentUsername, &balance) != EOF)
    {
        if (strcmp(currentUsername, username) == 0)
        {
            if (balance < amount)
            {
                printf(LIGHT_RED "\nInsufficient balance to complete the transaction.\n" RESET);
                usleep(800000);
                fclose(inputFile);
                fclose(tempFile);
                remove("temp.txt");
                return;
            }

            if (!isMpinCorrect(username, mpin))
            {

                printf(LIGHT_RED "\nTransaction failed: Incorrect MPIN. Please try again.\n" RESET);
                fclose(inputFile);
                fclose(tempFile);
                remove("temp.txt");
                usleep(800000);
                return;
            }

            balance -= amount;
        }
        else if (strcmp(currentUsername, rusername) == 0)
        {
            balance += amount;
            found = 1;
        }

        fprintf(tempFile, "%s %.2f\n", currentUsername, balance);
    }

    fclose(inputFile);
    fclose(tempFile);

    remove(FILE_DATABASE);
    rename("temp.txt", FILE_DATABASE);

    if (found)
    {

        logTransaction(username, -amount);
        logTransaction(rusername, amount);
        char outString[200];
        sprintf(outString, "\n%sTransaction successful: %sRs. %.2f%s has been sent to %s%s%s.%s\n", GREEN, BLUE, amount, RESET, MAGENTA, rusername, RESET, RESET);
        printf(outString);
        usleep(800000);

        usleep(800000);

        system("cls");
        displayUserInfo(username);
    }
}



//------------------------------------------------TRANSACTIONS LOGGING-----------------------------------------------------------
#include <errno.h>  

void logTransaction(char username[], float amount) {
    struct TransactionLog newLog = {
        .username = {0},  
        .amount = amount,
        .timestamp = {0}  
    };

    strncpy(newLog.username, username, MAX_USERNAME_LENGTH - 1);  
    newLog.username[MAX_USERNAME_LENGTH - 1] = '\0';  
    time_t now = time(NULL);
    strftime(newLog.timestamp, sizeof(newLog.timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));  

    FILE *file = fopen(FILE_TRANSACTIONS, "a");
    if (file == NULL) {
        fprintf(stderr, LIGHT_RED "\nError opening transaction log file: %s\n" RESET, strerror(errno));  
        return;
    }

    if (fprintf(file, "%s %.2f %s\n", newLog.username, newLog.amount, newLog.timestamp) < 0) {
        fprintf(stderr, LIGHT_RED "\nError writing to transaction log file.\n" RESET);
    }

    fclose(file);
}

void viewTransactions(char username[])
{
    FILE *file = fopen(FILE_TRANSACTIONS, "r");
    if (file != NULL)
    {
        printf(("\nTransaction History for User: %s %s %s\n", MAGENTA, username, RESET));
        printf("+----------------------+------------+--------------------------+\n");
        printf("|%s Amount %s              | %s Type %s     | %s Timestamp%s           |\n", LIGHT_YELLOW, RESET, LIGHT_YELLOW, RESET, LIGHT_YELLOW, RESET);
        printf("+----------------------+------------+--------------------------+\n");

        char currentUsername[MAX_USERNAME_LENGTH];
        float amount;
        char type[20];
        char timestamp[30];
        int found = 0;

        while (fscanf(file, "%s %f %[^\n]", currentUsername, &amount, timestamp) != EOF)
        {
            if (strcmp(currentUsername, username) == 0)
            {
                found = 1;
                if (amount > 0)
                {
                    strcpy(type, "Deposit");
                }
                else
                {
                    strcpy(type, "Withdrawal");
                    amount = -amount;
                }
                if (strcmp(type, "Deposit") == 0)
                {
                    printf("| Rs. %-16.2f | %s%-10s%s | %-19s |\n", amount, GREEN, type, RESET, timestamp);
                }
                else
                {
                    printf("| Rs. %-16.2f | %s%-10s%s | %-19s |\n", amount, LIGHT_RED, type, RESET, timestamp);
                }
            }
        }
        printf("+----------------------+------------+--------------------------+\n");

        fclose(file);

        if (!found)
        {
            printf(LIGHT_RED "\nNo transaction history found for User: %s\n" RESET, username);
        }

        waitForEnter();
        system("cls");
    }
    else
    {
        printf(LIGHT_RED "\nNo transaction history found for User: %s\n" RESET, username);
        usleep(800000);
        system("cls");
    }
    displayUserInfo(username);
}


//------------------------------------------------------------User Account Control --------------------------------------------------------------------------------

int registerAccount(struct User users[], int *numUsers)
{
    struct User newUser;

    while (1)
    {
        char newUsername[MAX_USERNAME_LENGTH];
        system("cls");
        printf(MAGENTA "\t\t\t\t\t\t\tREGISTRATION\n\n" RESET);
        printf(BLUE "\t\t\t\tEnter a username for your account: " RESET);
        scanf("%s", newUsername);

        int userIndex = findUserIndex(newUsername, users, *numUsers);
        if (userIndex != -1)
        {
            printf(LIGHT_YELLOW "\n\t\t\t\tUsername already exists. Please choose a different one.\n" RESET);
            waitForEnter();
        }
        else
        {
            strcpy(newUser.username, newUsername);
            break;
        }
    }
    int mpin;
    while (1)
    {
        printf(CYAN "\n\t\t\t\tCreate a 4-digit security PIN (MPIN): " RESET);
        scanf("%d", &mpin);
        if (!isValidMPIN(mpin))
        {
            printf(LIGHT_RED "\n\t\t\t\tInvalid MPIN. Please enter a 4-digit number.\n" RESET);
            waitForEnter();
        }
        else
        {
            newUser.mpin = mpin;
            break;
        }
    }

    users[*numUsers] = newUser;
    (*numUsers)++;

    saveUserToFile(newUser);
    createNewRecord(newUser.username);

    printf(GREEN "\n\t\t\t\tAccount registered successfully!" RESET);
    printf(CYAN "\n\t\t\t\tYou're now being logged in...\n" RESET);
    system("pause");
    return findUserIndex(newUser.username, users, *numUsers);
}

int getMPIN()
{
    
    int i = 0;
    int mpin;
    char ch;
    char password[MAX_PASSWORD_LENGTH];
    bool passwordEntered = false;

    printf(BLUE "\n\t\t\t\tEnter your MPIN: " RESET);
    // scanf("%d", &mpin);
    while (i < MAX_PASSWORD_LENGTH)
    {
        ch = getch();

        if (ch == 13)
        {
            break;
        }
        else if (ch == 8)
        {
            if (i > 0)
            {
                printf("\b \b");
                i--;
            }
        }
        else if (ch >= '0' && ch <= '9')
        {
            password[i] = ch;
            i++;
            printf("*");
            passwordEntered = true;
        }
    }

    password[i] = '\0';

    if (passwordEntered)
    {
        mpin = atoi(password);
    }
    else
    {
        printf(LIGHT_RED "\nInvalid MPIN.\n" RESET);
    }

    return mpin;
}

int login(struct User users[], int numUsers)
{
    system("cls");
    printf(MAGENTA"\t\t\t\tLOGIN\n\n"RESET);
    printf(BLUE "\t\t\t\tUsername: " RESET);
    char username[MAX_USERNAME_LENGTH];
    scanf("%s", username);
    int userIndex = findUserIndex(username, users, numUsers);
    if (userIndex != -1)
    {
        int chances = 3;
        while (chances > 0)
        {
            int mpin = getMPIN();
            if (!isValidMPIN(mpin))
            {
                printf(LIGHT_RED "\t\t\t\t\nInvalid MPIN format. Please enter a 4-digit number.\n" );
                printf(("\t\t\t\t[%sRemaining attempts:%s %s%d%s]\n\n", CYAN, RESET, LIGHT_YELLOW, chances - 1, RESET));
                waitForEnter();
                chances--;
            }
            else if (users[userIndex].mpin == mpin)
            {
                printf(GREEN "\n\t\t\t\tLogin successful. Welcome!\n" RESET);
                usleep(800000);
                return userIndex;
            }
            else
            {
                chances--;
                printf(LIGHT_RED "\n\t\t\t\tIncorrect MPIN. Please try again!\n" RESET);
                printf("\t\t\t\t[%sRemaining attempts:%s %s%d%s]\n\n", CYAN, RESET, LIGHT_YELLOW, chances, RESET);
            }
        }
    }
    else
    {
        return handleLoginFailure(users, numUsers);
    }
    printf(LIGHT_YELLOW "\n\t\t\t\tYou've run out of login attempts. Please try again later.\n" RESET);
    printf(LIGHT_RED "\n\t\t\t\tExiting...\n" RESET);
    return -1;
}

int handleLoginFailure(struct User users[], int numUsers)
{
    printf(LIGHT_RED "\n\t\t\t\tThe entered username does not exist.\n" RESET);

    printf("\n\t\t\t\t%sWould you like to:%s\n", MAGENTA, RESET);
    printf("%s\t\t\t\t[ 1 ]%s Retry Login\n", CYAN, RESET);
    printf("%s\t\t\t\t[ 2 ]%s Register\n", CYAN, RESET);
    printf("%s\t\t\t\t[ 3 ]%s Exit\n", CYAN, RESET);
    printf("\n\t\t\t\t%sEnter option:%s ", BLUE, RESET);

    int choice;
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        return login(users, numUsers);
    case 2:
        registerAccount(users, &numUsers);
        return numUsers - 1;
    default:
        printf(LIGHT_RED "\n\t\t\t\tExiting...\n" RESET);
        return initMenu();
    }
}

// ----------------------------------------------------- MENUS -----------------------------------------------------------------------

void displayMainMenu()
{
    printf("\n%sWhat would you like to do?%s\n", MAGENTA, RESET);
    printf("%s[ 1 ]%s Send Money\n", CYAN, RESET);
    printf("%s[ 2 ]%s Load Money\n", CYAN, RESET);
    printf("%s[ 3 ]%s View Transaction History\n", CYAN, RESET);
    printf("%s[ 4 ]%s Logout\n", CYAN, RESET);
    printf("%s[ 5 ]%s Delete account\n", CYAN, RESET);

    printf("\n%sEnter option:%s ", BLUE, RESET);
}


int initMenu()
{
    int option;
    int numUsers = 0;
    loadUserData(users, &numUsers);

    while (1)
    {
        system("cls");
        printf("\n%sWelcome to the Banking System\n", MAGENTA);
        printf("%s-------------------------------\n\n", RESET);
        printf("%s[ 1 ]%s Login\n", CYAN, RESET);
        printf("%s[ 2 ]%s Register\n", CYAN, RESET);
        printf("%s[ 3 ]%s Exit\n", CYAN, RESET);

        printf("\n%sEnter option:%s ", BLUE, RESET);
        scanf("%d", &option);

        switch (option)
        {
        case 1:
        {
            int userIndex = login(users, numUsers);
            if (userIndex != -1)
            {
                return innerMenu(users[userIndex].username, userIndex);
            }
            break;
        }
        case 2:
        {
            int userIndex = registerAccount(users, &numUsers);
            if (userIndex != -1)
            {
                return innerMenu(users[userIndex].username, userIndex);
            }
            break;
        }
        case 3:
            printf(LIGHT_RED "\nExiting...\n" RESET);
            usleep(800000);
            exit(0);

        default:
            printf(LIGHT_RED "\nInvalid option. Please try again.\n" RESET);
            waitForEnter();
            break;
        }
    }
}

int innerMenu(char username[], int userIndex)
{
    int option;
    int numUsers = 0;
    loadUserData(users, &numUsers);

    while (1)
    {
        system("cls");
        displayUserInfo(username);
        displayMainMenu();
        scanf("%d", &option);

        switch (option)
        {
        case 1:
        {
            printf(BLUE "\nEnter the username of the recipient: " RESET);

            char receiverUsername[MAX_USERNAME_LENGTH];
            scanf("%s", receiverUsername);
            sendMoney(users[userIndex].username, receiverUsername);
            break;
        }
        case 2:
        {
            loadMoney(users[userIndex].username);
            break;
        }
        case 3:
        {
            viewTransactions(users[userIndex].username);
            break;
        }
        case 4:
        {
            printf(LIGHT_YELLOW "\nLogging you out...\n" RESET);
            usleep(500000);
            initMenu();
            break;
        }
        case 5:
            if (confirmDeletion())
            {
                deleteUser(users, users[userIndex].username, numUsers);
                printf(GREEN "\nAccount deleted successfully. Thank you for using our service!\n" RESET);
                waitForEnter();
            }
            else
            {
                printf(CYAN "\nAccount deletion canceled.\n" RESET);
                usleep(800000);
            }
            break;

        default:
            printf(LIGHT_RED "\nInvalid option. Please try again.\n" RESET);
            waitForEnter();
            break;
        }
    }
}