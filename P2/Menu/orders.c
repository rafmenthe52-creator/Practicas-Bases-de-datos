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
    int ordernumber;
    

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
                fprintf(stdin, "Enter the order number to get the order details > ");
                fscanf(stdin, "%d", &ordernumber);
                ShowOrderDetails(dbc, (SQLINTEGER)ordernumber);
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
               " (2) List Orders in range\n"
               " (3) Show order details\n"
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

void ListOrdersByRange(SQLHDBC dbc,char *start_date, char *end_date)
{
    SQLHENV env = NULL;
    SQLHDBC dbc = NULL;
    SQLHSTMT stmt = NULL;

    SQLRETURN ret;

    SQLINTEGER ordernumber;
    SQLINTEGER offset;

    char orderdate[BufferLength] = "\0";
    char shippeddate[BufferLength] = "\0";

    SQLLEN indicator1;
    SQLLEN indicator2;
    SQLLEN indicator3;
    SQLLEN indicator4;
    SQLLEN indicator5;

    int page = 0;
    int i;
    int has_next;
    int error = 0;

    char option;

    /* Connect to database */
    ret = odbc_connect(&env, &dbc);

    if (!SQL_SUCCEEDED(ret)) {
        printf("Error connecting to database.\n");
        return;
    }

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
        "SELECT orderNumber, orderDate, shippedDate "
        "FROM orders "
        "WHERE orderDate BETWEEN ? AND ? "
        "ORDER BY orderNumber "
        "LIMIT 11 OFFSET ?",
        SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLPrepare", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return;
    }

    /* Bind start date */
    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
        SQL_C_CHAR, SQL_TYPE_DATE, 10, 0,
        start_date, 11, &indicator1);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindParameter", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return;
    }

    indicator1 = SQL_NTS;

    /* Bind end date */
    ret = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT,
        SQL_C_CHAR, SQL_TYPE_DATE, 10, 0,
        end_date, 11, &indicator2);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindParameter", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return;
    }

    indicator2 = SQL_NTS;

    /* Bind offset */
    ret = SQLBindParameter(stmt, 3, SQL_PARAM_INPUT,
        SQL_C_SLONG, SQL_INTEGER, 0, 0,
        &offset, 0, &indicator3);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindParameter", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return;
    }

    /* Bind result columns */
    ret = SQLBindCol(stmt, 1, SQL_C_SLONG,
        &ordernumber, sizeof(ordernumber), &indicator3);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindCol", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return;
    }

    ret = SQLBindCol(stmt, 2, SQL_C_CHAR,
        orderdate, sizeof(orderdate), &indicator4);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindCol", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return;
    }

    ret = SQLBindCol(stmt, 3, SQL_C_CHAR,
        shippeddate, sizeof(shippeddate), &indicator5);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindCol", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return;
    }

    /* Pagination */
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

        printf("\nOrders between %s and %s\n", start_date, end_date);
        printf("--------------------------------------------------\n");
        printf("%-15s %-15s %-15s\n",
            "Order number", "Order date", "Shipped date");
        printf("--------------------------------------------------\n");

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

            printf("%-15d %-15s ", ordernumber, orderdate);

            if (indicator5 == SQL_NULL_DATA) {
                printf("%-15s\n", "Not shipped");
            }
            else {
                printf("%-15s\n", shippeddate);
            }

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
                printf("No orders found in this date range.\n");
            }

            SQLCloseCursor(stmt);
            break;
        }

        printf("--------------------------------------------------\n");

        if (page > 0) {
            printf("< Previous page");
        }

        if (page > 0 && has_next) {
            printf(" |  > Next page");
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

void ShowOrderDetails(SQLHDBC dbc, SQLINTEGER ordernumber);



//Still in progress.