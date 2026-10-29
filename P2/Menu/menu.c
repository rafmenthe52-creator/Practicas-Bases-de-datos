#include <stdio.h>
#include <stdlib.h>
#include <odbc.h>
#include <sql.h>
#include <sqlext.h>

/* #include "menu_template.h" */
/* #ifndef A2BIS_MENU_TEMPLATE_H */
/* #define A2BIS_MENU_TEMPLATE_H */

/* Prototypes:
 * Since in this example this functions are not used outside menu_template
 * it does not make sense to create an include file and export them
 * BUT in your code it will make sense to break this file in many files
 * and create several include files with the prototypes
 *
 * A static function in C is a function that has a scope that is limited
 * to its object file. This means that the static function is only
 * visible in its object file. Therefore,
 * you need to remove the static keyword from the declaration
 * if the function is declared in a file and used in another
 * */

static int ShowMainMenu();
static void ShowProductsMenu();
static int ShowProductsSubMenu();
static void ShowOrdersMenu();
static int ShowOrdersSubMenu();
static void ShowCustomersMenu();
static int ShowCustomersSubMenu();


/* #endif //A2BIS_MENU_TEMPLATE_H */

/**
 * @file menu_template.c
 * @author rmarabini
 * @date 15 April 2020
 * @brief File containing an example of a menu system with submenus
 *
 * The following program allows a user to select a nursery rhyme
 * or a fairy tail from a menu. The menu has two levels, the top
 * one allows to select between fairy tails or nursery rhymes. The
 * second level (sub-menus) allow to select either a particular
 * rhyme or tail.
 *
 * @see https://bytes.com/topic/c/answers/567136-creating-menus-sub-menus
 */


/**
 * @brief Entry point, shows and process main menu.
 *
 * This function implements a loop that prints the main
 * menu, analyze the user selection and calls the
 * corresponding submenu
 *
 * @return 0 if no error
 * @author rmarabini
 */
int main(void) {
    SQLHENV env = NULL;
    SQLHDBC dbc = NULL;
    int nChoice = 0;

    if (!SQL_SUCCEEDED(odbc_connect(&env, &dbc))) {
        fprintf(stderr, "Error conectando a la base de datos.\n");
        return EXIT_FAILURE;
    }

    do {
        nChoice = ShowMainMenu();
        switch (nChoice) {
            case 1: {
                ShowProductsMenu();
            }
                break;

            case 2: {
                ShowOrdersMenu();
            }
                break;
            
            case 3: {
                ShowCustomersMenu();
            }
                break;

            case 4: {
                printf("Bye Bye\n\n");
            }
                break;
        }
    } while (nChoice != 4);

    (void) odbc_disconnect(env, dbc);
    return 0;
}


/**
 * @brief prints main menu and allows to select an option.
 *
 *
 * @return selected entry in the menu
 * @author rmarabini
 */

int ShowMainMenu() {
    int nSelected = 0;
    char buf[16]; /* buffer for reading user input
                     in theory we only need 1 char
                     but a large buffer will handle
                     more gracefully wrong inputs
                     as 'qwerty' */

    do {
        printf("This is a nursery rhyme and fairy stories program\n");
        printf("You can choose to display a number of different nursery rhymes and fairy stories\n\n");

        printf(" (1) Products\n"
               " (2) Orders\n"
               " (3) Customers\n"
               " (4) Exit\n\n"
               "Enter a number that corresponds to your choice > ");
        if (!fgets(buf, 16, stdin))
            /* reading input failed, give up: */
            nSelected =0;
        else
            /* have some input, convert it to integer: */
            nSelected = atoi(buf);
        printf("\n");

        if ((nSelected < 1) || (nSelected > 4)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }
    } while ((nSelected < 1) || (nSelected > 4));

    return nSelected;
}

/**
 * @brief Shows and processes the rhyme menu.
 *
 * This function implements a loop that prints the rhyme
 * menu, analyze the user selection and prints
 * the selected rhyme
 * @return void
 * @author rmarabini
 */

void ShowProductsMenu() {
    int nChoice = 0;
    int productnumber;
    char *productname;

    do {
        nChoice = ShowProductsSubMenu();
        switch (nChoice) {

            case 1: {
                fprintf(stdin, "Enter the product number to check stock > ");
                fscanf(stdin, "%d", &productnumber);
                StockProducts(productnumber);
            }
                break;

            case 2: {
                fprintf(stdin, "Enter the product name to find > ");
                fscanf(stdin, "%s", productname);
                FindProducts(productname);
            }
                break;

            case 3: {
                printf("Bye Bye\n\n");
            }
                break;
        }
    } while (nChoice != 3);

}

/**
 * @brief prints rhyme menu and allows to select an option.
 *
 * @return selected option
 * @author rmarabini
 */

 int ShowProductsSubMenu() {
    int nSelected = 0;
    char buf[16];

    do {

        printf(" (1) Stock\n"
               " (2) Find\n"
               " (3) Back\n\n");

        printf("Enter a number that corresponds to your choice > ");
        if (!fgets(buf, 16, stdin))
            /* reading input failed, give up: */
            nSelected =0;
        else
            /* have some input, convert it to integer: */
            nSelected = atoi(buf);
        printf("\n");


        if ((nSelected < 1) || (nSelected > 3)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }
    } while ((nSelected < 1) || (nSelected > 3));

    return nSelected;
}

/**
 * @brief Shows and processes the fairy menu.
 *
 * This function implements a loop that prints the fairy
 * menu, analyze the user selection and prints
 * the selected fairy tail
 *
 * @return selected option
 * @author rmarabini
 */

void ShowOrdersMenu() {
    int nChoice = 0;
    char *startdate;
    char *enddate;
    int ordernumber;

    do {
        nChoice = ShowOrdersSubMenu();
        switch (nChoice) {
            case 1: {
                OpenOrders();
            }
                break;

            case 2: {
                fprintf(stdin, "Enter the start date (YYYY-MM-DD) > ");
                fscanf(stdin, "%s", startdate);
                fprintf(stdin, "Enter the end date (YYYY-MM-DD) > ");
                fscanf(stdin, "%s", enddate);
                RangeOrders(startdate, enddate);
            }
                break;

            case 3: {
                fprintf(stdin, "Enter the order number > ");
                fscanf(stdin, "%d", &ordernumber);
                DetailOrders(ordernumber);
            }
                break;

            case 4: {
                printf("Bye Bye\n\n");
            }
                break;
        }
    } while (nChoice != 4);

}

/**
 * @brief prints the fairy menu and allows to select an option.
 *
 * @return selected option
 * @author rmarabini
 */
 int ShowOrdersSubMenu() {
    int nSelected = 0;
    char buf[16];

    do {

        printf(" (1) Open\n"
               " (2) Range\n"
               " (3) Detail\n"
               " (4) Back\n\n");

        printf("Enter a number that corresponds to your choice > ");
        if (!fgets(buf, 16, stdin))
            /* reading input failed, give up: */
            nSelected =0;
        else
            /* have some input, convert it to integer: */
            nSelected = atoi(buf);
        printf("\n");

        if ((nSelected < 1) || (nSelected > 4)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }
    } while ((nSelected < 1) || (nSelected > 4));

    return nSelected;
}

/**
 * @brief Shows and processes the fairy menu.
 *
 * This function implements a loop that prints the fairy
 * menu, analyze the user selection and prints
 * the selected fairy tail
 *
 * @return selected option
 * @author rmarabini
 */

void ShowCustomersMenu() {
    int nChoice = 0;
    char *customername;
    int customernumber;
    

    do {
        nChoice = ShowCustomersSubMenu();
        switch (nChoice) {
            case 1: {
                fprintf(stdin, "Enter the customer name to find > ");
                fscanf(stdin, "%s", customername);
                FindCustomer(customername);
            }
                break;

            case 2: {
                fprintf(stdin, "Enter the customer number to list products > ");
                fscanf(stdin, "%d", &customernumber);
                ListProducts(customernumber);
            }
                break;

            case 3: {
                fprintf(stdin, "Enter the customer number to get balance > ");
                fscanf(stdin, "%d", &customernumber);
                Balance(customernumber);
            }
                break;

            case 4: {
                printf("Bye Bye\n\n");
            }
                break;
        }
    } while (nChoice != 4);

}

/**
 * @brief prints the fairy menu and allows to select an option.
 *
 * @return selected option
 * @author rmarabini
 */
 int ShowCustomersSubMenu() {
    int nSelected = 0;
    char buf[16];

    do {

        printf(" (1) Find\n"
               " (2) List Products\n"
               " (3) Balance\n"
               " (4) Back\n\n");

        printf("Enter a number that corresponds to your choice > ");
        if (!fgets(buf, 16, stdin))
            /* reading input failed, give up: */
            nSelected =0;
        else
            /* have some input, convert it to integer: */
            nSelected = atoi(buf);
        printf("\n");

        if ((nSelected < 1) || (nSelected > 4)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }
    } while ((nSelected < 1) || (nSelected > 4));

    return nSelected;
}

     

int FindCustomers(char *name)
{
    SQLHENV env = SQL_NULL_HENV;
    SQLHDBC dbc = SQL_NULL_HDBC;
    SQLHSTMT stmt = NULL;
    int ret;
    char query[] = "SELECT customerNumber, customerName, contactFirstName, contactLastName "
    "FROM customers "
    "WHERE contactFirstName LIKE ? OR contactLastName LIKE ? "
    "ORDER BY customerNumber";


    ret = odbc_connect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return ret;
    }

    ret = SQLPrepare(stmt, (SQLCHAR*) query, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return ret;
    }

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &name, 0, NULL);
                                
        
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}







void FindProducts(SQLHDBC dbc) {
    SQLHSTMT stmt = NULL;
    char input[70] = "\0";
    char pattern[75] = "\0";
    char code[16] = "\0", name[71] = "\0";

    printf("Enter product name > ");
    (void) getchar(); /* Limpiar buffer tras enteros previos si fuera necesario */
    if (!fgets(input, sizeof(input), stdin)) return;
    input[strcspn(input, "\r\n")] = '\0'; /* Eliminar salto de línea */

    snprintf(pattern, sizeof(pattern), "%%%s%%", input);

    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt))) return;

    if (SQL_SUCCEEDED(SQLPrepare(stmt, (SQLCHAR*)
        "SELECT productcode, productname FROM products WHERE productname ILIKE ? ORDER BY productcode", SQL_NTS))) {

        (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
                                strlen(pattern), 0, pattern, 0, NULL);

        if (SQL_SUCCEEDED(SQLExecute(stmt))) {
            (void) SQLBindCol(stmt, 1, SQL_C_CHAR, code, sizeof(code), NULL);
            (void) SQLBindCol(stmt, 2, SQL_C_CHAR, name, sizeof(name), NULL);

            while (SQL_SUCCEEDED(SQLFetch(stmt))) {
                printf("%s\t%s\n", code, name);
            }
            printf("\n");
        }
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}