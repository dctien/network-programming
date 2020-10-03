#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"

void readFile(FILE *fin, node **root);
void writeFile(FILE *fin, node *root);
node *checkUsername(node *root, elementtype e);
void printNode(node *root);

int main()
{
    FILE *fprt;
    fprt = fopen("taikhoan.txt", "r+");
    if (fprt == NULL)
    {
        printf("File not found!\n");
        return 0;
    }
    node *root = NULL;
    elementtype element;
    node *cur = NULL;

    readFile(fprt, &root);
    int select;
    do
    {
        printf("---------------------------------------\n");
        printf("USER MANAGEMENT PROGRAM\n");
        printf("---------------------------------------\n");
        printf("1. Register\n");
        printf("2. Activate\n");
        printf("3. Sign in\n");
        printf("4. Search\n");
        printf("5. Change password\n");
        printf("6. Sign out\n");
        printf("---------------------------------------\n");
        printf("Your choice(1-6, other to quit): ");
        scanf("%d", &select);
        switch (select)
        {
        case 1: //Register
            while ((getchar()) != '\n')
                ;
            printf("\n---------Register---------\n");
            printf("Enter username : ");
            scanf("%s", element.user_name);
            while ((cur = checkUsername(root, element)) != NULL)
            {
                while ((getchar()) != '\n')
                    ;
                printf("Account is existed!\n");
                printf("Enter username: ");
                scanf("%s", element.user_name);
            }
            while ((getchar()) != '\n')
                ;
            printf("Password : ");
            scanf("%s", element.password);
            element.status = 2;
            insertAtEnd(&root, element);
            printf("Successful registration\n");
            fprintf(fprt, "%s %s %d\n", element.user_name, element.password, element.status);
            fclose(fprt);
            break;
        case 2: //Activate
            if (fprt != NULL)
                fclose(fprt);
            while ((getchar()) != '\n')
                ;
            printf("\n---------Actiate---------\n");
            printf("Enter username : ");
            scanf("%s", element.user_name);
            while ((cur = checkUsername(root, element)) == NULL)
            {
                while ((getchar()) != '\n')
                    ;
                printf("Account is not existed!\n");
                printf("Enter username: ");
                scanf("%s", element.user_name);
            }

            while ((getchar()) != '\n')
                ;
            printf("Password : ");
            scanf("%s", element.password);
            while (strcmp(cur->element.password, element.password) != 0)
            {
                printf("Wrong password! Enter password: ");
                scanf("%s", element.password);
            }
            int dem = 0;
            char code[20];
            char acCode[] = "LTM121216";
            while ((getchar()) != '\n')
                ;
            printf("Activation code : ");
            scanf("%s", code);
            while (strcmp(code, acCode) != 0)
            {
                dem++;
                if (dem == 3)
                {
                    printf("Account %s have been blocked!\n", element.user_name);
                    cur->element.status = 0;
                    writeFile(fprt, root);
                    fclose(fprt);
                    break;
                }
                printf("Wrong activation code! Enter Activation code: ");
                scanf("%s", element.password);
            }
            if (dem < 3)
            {
                if (cur->element.status == 2)
                {
                    fprt = fopen("taikhoan.txt", "r+");
                    cur->element.status = 1;
                    writeFile(fprt, root);
                    printf("Successfully sign in\n");
                    fclose(fprt);
                }
                else if (cur->element.status == 1)
                {
                    printf("Account has been activated before\n");
                }
                else
                    printf("The account %s does not exist or has been locked!\n", cur->element.user_name);
            }
            break;

        case 3: //Sign in
            getchar();
            printf("\n---------Sign in---------\n");
            printf("Enter username: ");
            scanf("%s", element.user_name);
            // Check username
            while ((cur = checkUsername(root, element)) == NULL)
            {
                puts("Username is error!");
                printf("Enter username: ");
                scanf("%s", element.user_name);
            }
            printf("Enter password: ");
            scanf("%s", element.password);
            // Check password
            int count = 0;
            while (strcmp(cur->element.password, element.password) != 0)
            {
                count++;
                if (count == 3)
                {
                    printf("***Account is blocked@@\n");
                    cur->element.status = 0;
                    fprt = fopen("taikhoan.txt", "w");
                    writeFile(fprt, root);
                    fclose(fprt);
                    break;
                }
                printf("Wrong password! Enter password: ");
                scanf("%s", element.password);
            }
            if (count < 3)
            {
                if (cur->element.status == 1)
                {
                    cur->element.login = 1;
                    printf("***Sign in successfully\n");
                }
                else
                    puts("***Account does not exist or is blocked");
            }
            break;
        case 4: //Search
            getchar();
            printf("\n---------Search---------\n");
            printf("Enter username: ");
            scanf("%s", element.user_name);
            cur = checkUsername(root, element);
            if (cur == NULL)
            {
                puts("***Can not find this account");
            }
            else if (cur->element.status == 1)
            {
                puts("Account is active");
            }
            else
            {
                puts("***Account is blocked or not activated");
            }

            break;
        case 5: //Change password

            getchar();
            printf("\n---------Change password---------\n");
            printf("Enter username: ");
            scanf("%s", element.user_name);
            // Check username
            while ((cur = checkUsername(root, element)) == NULL)
            {
                puts("Username is not!");
                printf("Enter username: ");
                scanf("%s", element.user_name);
            } 
            if( cur->element.login==0){
                printf("The account is not sign in\n");
                break;
            }
            getchar();
            printf("Enter password : ");
            scanf("%s", element.password);
            while (strcmp(cur->element.password, element.password) != 0)
            {
                printf("Wrong password! Enter password: ");
                scanf("%s", element.password);
            }
            printf("Enter new pasword: ");
            scanf("%s", cur->element.password);
            printf("Pasword has changed\n");
            fprt = fopen("taikhoan.txt", "w");
            writeFile(fprt,cur);
            fclose(fprt); 
            break;
        case 6: //Sign out
            getchar();
            printf("\n---------Sign out---------\n");
            printf("Enter username: ");
            scanf("%s", element.user_name);
            cur = checkUsername(root, element);
            if (cur == NULL)
            {
                puts("***Can not find this account");
            }
            else if (cur->element.login == 1)
            {
                cur->element.login = 0;
                printf("***Goodbye %s\n", cur->element.user_name);
            }
            else
            {
                puts("Account is not sign in"); 
            }
            break;
        default:
            puts("THANKYOU!");
            break;
        }
    } while (select == 1 || select == 2 || select == 3 || select == 4 || select == 5 || select == 6);
    fclose(fprt);
}

void readFile(FILE *fin, node **root)
{
    (*root) = freeList(*root);
    elementtype element;
    while ((fscanf(fin, "%s%s%d", element.user_name, element.password, &element.status)) != EOF)
    {
        element.login = 0;
        insertAtEnd(root, element);
    }
}

void writeFile(FILE *fin, node *root)
{
    node *tmp = root;
    while (tmp != NULL)
    {
        fprintf(fin, "%s %s %d\n", tmp->element.user_name, tmp->element.password, tmp->element.status);
        tmp = tmp->next;
    }
}

node *checkUsername(node *root, elementtype e)
{
    node *temp = root;
    while (temp != NULL)
    {
        if (strcmp(temp->element.user_name, e.user_name) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void printNode(node *root)
{
    node *tmp = root;
    while (tmp != NULL)
    {
        printf("%s-%s-%d\n", tmp->element.user_name, tmp->element.password, tmp->element.status);
        tmp = tmp->next;
    }
}