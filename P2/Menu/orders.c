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

void ListOpenOrders(SQLHDBC dbc)
{
    SQLHENV env = NULL;
    SQLHSTMT stmt = NULL;

    SQLRETURN ret;

    SQLINTEGER ordernumber;
    SQLINTEGER offset;

    SQLLEN indicator;

    int page = 0;
    int i;
    int has_next;
    int error = 0;

    char option;

    /* Allocate statement */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLAllocHandle", dbc, SQL_HANDLE_DBC);
        odbc_disconnect(env, dbc);
        return;
    }

    /* Prepare query */
    ret = SQLPrepare(stmt,
        (SQLCHAR *)
        "SELECT orderNumber "
        "FROM orders "
        "WHERE shippedDate IS NULL "
        "ORDER BY orderNumber "
        "LIMIT 11 OFFSET ?",
        SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLPrepare", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return;
    }

    /* Bind offset parameter */
    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
        SQL_C_SLONG, SQL_INTEGER, 0, 0,
        &offset, 0, NULL);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindParameter", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return;
    }

    /* Bind result column */
    ret = SQLBindCol(stmt, 1, SQL_C_SLONG,
        &ordernumber, sizeof(ordernumber), &indicator);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindCol", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return;
    }

    /* Paginación */
    while (1) {

        offset = page * 10;
        i = 0;
        has_next = 0;

        ret = SQLExecute(stmt);

        if (!SQL_SUCCEEDED(ret)) {
            odbc_extract_error("SQLExecute", stmt, SQL_HANDLE_STMT);
            error = 1;
            break;
        }

        printf("\nOpen orders\n");
        printf("--------------------------\n");

        /* Display the first 10 orders */
        while (i < 10) {

            ret = SQLFetch(stmt);

            if (ret == SQL_NO_DATA) {
                break;
            }

            if (!SQL_SUCCEEDED(ret)) {
                odbc_extract_error("SQLFetch", stmt, SQL_HANDLE_STMT);
                error = 1;
                break;
            }

            printf("Order number: %d\n", ordernumber);

            i++;
        }

        if (error) {
            SQLCloseCursor(stmt);
            break;
        }

        /* Check whether an 11th order exists */
        if (i == 10) {

            ret = SQLFetch(stmt);

            if (SQL_SUCCEEDED(ret)) {
                has_next = 1;
            }
            else if (ret != SQL_NO_DATA) {
                odbc_extract_error("SQLFetch", stmt, SQL_HANDLE_STMT);
                error = 1;
            }
        }

        if (error) {
            SQLCloseCursor(stmt);
            break;
        }

        if (i == 0) {
            if (page == 0) {
                printf("There are no open orders.\n");
            }
            SQLCloseCursor(stmt);
            break;
        }

        printf("--------------------------\n");

        if (page > 0) {
            printf("< Previous page");
        }

        if (page > 0 && has_next) {
            printf(" | > Next page");
        }

        if (page == 0 && !has_next) {
            printf("No more pages.");
            SQLCloseCursor(stmt);
            break;
        }

        printf("\nEnter an option (<, > or q): ");
        scanf(" %c", &option);

        SQLCloseCursor(stmt);

        if (option == '>' && has_next) {
            page++;
        }
        else if (option == '<' && page > 0) {
            page--;
        }
        else {
            break;
        }
    }

    /* Free resources */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    odbc_disconnect(env, dbc);
}

void ListOrdersByRange(SQLHDBC dbc, char *start_date, char *end_date);
void ShowOrderDetails(SQLHDBC dbc, SQLINTEGER ordernumber);



//Still in progress.