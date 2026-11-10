#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "customers.h"
#include "odbc.h"

#define BufferLength 512

static int ShowOrdersSubMenu(void);

/**
 * @brief Shows the Orders submenu and processes user selection.
 *
 *
 * @return selected option
 * @author Rafael Velasco Wagner
 */

void ShowOrdersMenu(SQLHDBC dbc) {
    int nChoice = 0;
    char *orderstart, *orderfinish;
    int customernumber;
    

    do {
        nChoice = ShowOrdersSubMenu();
        switch (nChoice) {
            case 1: {
                ListOpenOrders(dbc);
            }
                break;

            case 2: {
                fprintf(stdin, "Enter the date from which you want to see the orders in the following way (dd/mm/yyyy)> ");
                fscanf(stdin, "%c", orderstart);
                fprintf(stdin, "Enter the date until which you want to see the orders in the following way (dd/mm/yyyy)> ");
                fscanf(stdin, "%c", orderfinish);
                ListOrdersByRange(dbc, orderstart, orderfinish);
            }
                break;

            case 3: {
                fprintf(stdin, "Enter the customer number to get balance > ");
                fscanf(stdin, "%d", &customernumber);
                Balance(dbc, customernumber);
            }
                break;

            case 4: {
                printf("Bye Bye\n\n");
            }
                break;
        }
    } while (nChoice != 4);

}

int ShowOrdersSubMenu() {
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
            nSelected = 0;
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

void ListOpenOrders(SQLHDBC dbc);
void ListOrdersByRange(SQLHDBC dbc, char *start_date, char *end_date);
void ShowOrderDetails(SQLHDBC dbc, SQLINTEGER ordernumber);



//Still in progress.