#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "orders.h"
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
    char input[BufferLength] = "\0";
    char orderstart[16] = "\0";
    char orderfinish[16] = "\0";
    int ordernumber = 0;

    do {
        nChoice = ShowOrdersSubMenu();
        switch (nChoice) {
            case 1: {
                ListOpenOrders(dbc);
            }
                break;

            case 2: {
                printf("Enter dates (YYYY-MM-DD - YYYY-MM-DD) > ");
                if (fgets(input, (int) sizeof(input), stdin) != NULL) {
                    if (sscanf(input, "%15s - %15s", orderstart, orderfinish) == 2) {
                        ListOrdersByRange(dbc, orderstart, orderfinish);
                    } else {
                        printf("Invalid date format. Expected YYYY-MM-DD - YYYY-MM-DD\n\n");
                    }
                }
            }
                break;

            case 3: {
                printf("Enter the order number to get the order details > ");
                if (fgets(input, (int) sizeof(input), stdin) != NULL) {
                    ordernumber = atoi(input);
                    ShowOrderDetails(dbc, (SQLINTEGER) ordernumber);
                }
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

        printf(" (1) List open orders\n"
               " (2) List Orders in range\n"
               " (3) Show order details\n"
               " (4) Back\n\n");

        printf("Enter a number that corresponds to your choice > ");
        if (!fgets(buf, (int) sizeof(buf), stdin))
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
    SQLHSTMT stmt = NULL;
    SQLRETURN ret;
    SQLINTEGER ordernumber = 0;
    int found = 0;

    /* Allocate statement */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLAllocHandle", dbc, SQL_HANDLE_DBC);
        return;
    }

    /* Execute query (no user parameters needed) */
    ret = SQLExecDirect(stmt,
        (SQLCHAR *)
        "SELECT orderNumber "
        "FROM orders "
        "WHERE shippedDate IS NULL "
        "ORDER BY orderNumber",
        SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    /* Bind result column */
    ret = SQLBindCol(stmt, 1, SQL_C_SLONG, &ordernumber, 0, NULL);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindCol", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%d\n", ordernumber);
        found = 1;
    }

    if (ret != SQL_NO_DATA) {
        odbc_extract_error("SQLFetch", stmt, SQL_HANDLE_STMT);
    } else if (!found) {
        printf("There are no open orders.\n");
    }

    printf("\n");

    /* Free resources */
    (void) SQLCloseCursor(stmt);
    (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void ListOrdersByRange(SQLHDBC dbc, char *start_date, char *end_date)
{
    SQLHSTMT stmt = NULL;
    SQLRETURN ret;

    SQLINTEGER ordernumber = 0;
    char orderdate[BufferLength] = "\0";
    char shippeddate[BufferLength] = "\0";

    SQLLEN indShipped = 0;
    int found = 0;

    /* Allocate statement */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLAllocHandle", dbc, SQL_HANDLE_DBC);
        return;
    }

    /* Prepare query */
    ret = SQLPrepare(stmt,
        (SQLCHAR *)
        "SELECT ordernumber, orderdate, shippeddate "
        "FROM orders "
        "WHERE orderdate BETWEEN ? AND ? "
        "ORDER BY ordernumber",
        SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLPrepare", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    /* Bind start date */
    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
        SQL_C_CHAR, SQL_VARCHAR, 10, 0,
        start_date, 0, NULL);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindParameter", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    /* Bind end date */
    ret = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT,
        SQL_C_CHAR, SQL_VARCHAR, 10, 0,
        end_date, 0, NULL);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindParameter", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    /* Execute query */
    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLExecute", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    /* Bind result columns */
    (void) SQLBindCol(stmt, 1, SQL_C_SLONG, &ordernumber, 0, NULL);
    (void) SQLBindCol(stmt, 2, SQL_C_CHAR, orderdate, sizeof(orderdate), NULL);
    (void) SQLBindCol(stmt, 3, SQL_C_CHAR, shippeddate, sizeof(shippeddate), &indShipped);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        if (indShipped == SQL_NULL_DATA) {
            printf("%d\t%s\tNot shipped\n", ordernumber, orderdate);
        } else {
            printf("%d\t%s\t%s\n", ordernumber, orderdate, shippeddate);
        }
        found = 1;
    }

    if (ret != SQL_NO_DATA) {
        odbc_extract_error("SQLFetch", stmt, SQL_HANDLE_STMT);
    } else if (!found) {
        printf("No orders found in this date range.\n");
    }

    printf("\n");

    /* Free resources */
    (void) SQLCloseCursor(stmt);
    (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void ShowOrderDetails(SQLHDBC dbc, SQLINTEGER ordernumber)
{
    SQLHSTMT stmt = NULL;
    SQLRETURN ret;

    SQLINTEGER quantity = 0;
    SQLDOUBLE total = 0.0;
    SQLDOUBLE price = 0.0;

    char orderdate[BufferLength] = "\0";
    char status[BufferLength] = "\0";
    char productcode[BufferLength] = "\0";

    /* Allocate statement */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLAllocHandle", dbc, SQL_HANDLE_DBC);
        return;
    }

    /* Prepare query (una única consulta con función ventana para el total) */
    ret = SQLPrepare(stmt,
        (SQLCHAR *)
        "SELECT o.orderDate, o.status, "
        "       SUM(od.quantityOrdered * od.priceEach) OVER (), "
        "       od.productCode, od.quantityOrdered, od.priceEach "
        "FROM orders o "
        "JOIN orderdetails od ON o.orderNumber = od.orderNumber "
        "WHERE o.orderNumber = ? "
        "ORDER BY od.orderLineNumber",
        SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLPrepare", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    /* Bind order number */
    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
        SQL_C_SLONG, SQL_INTEGER, 0, 0,
        &ordernumber, 0, NULL);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindParameter", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    /* Execute query */
    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLExecute", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    /* Bind result columns */
    (void) SQLBindCol(stmt, 1, SQL_C_CHAR, orderdate, sizeof(orderdate), NULL);
    (void) SQLBindCol(stmt, 2, SQL_C_CHAR, status, sizeof(status), NULL);
    (void) SQLBindCol(stmt, 3, SQL_C_DOUBLE, &total, 0, NULL);
    (void) SQLBindCol(stmt, 4, SQL_C_CHAR, productcode, sizeof(productcode), NULL);
    (void) SQLBindCol(stmt, 5, SQL_C_SLONG, &quantity, 0, NULL);
    (void) SQLBindCol(stmt, 6, SQL_C_DOUBLE, &price, 0, NULL);

    /* Fetch first row */
    ret = SQLFetch(stmt);
    if (ret == SQL_NO_DATA) {
        printf("Order %d not found.\n\n", ordernumber);
        (void) SQLCloseCursor(stmt);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLFetch", stmt, SQL_HANDLE_STMT);
        (void) SQLCloseCursor(stmt);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    /* Línea 1: fecha del pedido y estado */
    printf("%s\t%s\n", orderdate, status);
    /* Línea 2: coste total del pedido */
    printf("%.2f\n", total);

    /* Líneas siguientes: cada producto del pedido */
    do {
        printf("%s\t%d\t%.2f\n", productcode, quantity, price);
    } while (SQL_SUCCEEDED(ret = SQLFetch(stmt)));

    if (ret != SQL_NO_DATA) {
        odbc_extract_error("SQLFetch", stmt, SQL_HANDLE_STMT);
    }

    printf("\n");

    /* Free resources */
    (void) SQLCloseCursor(stmt);
    (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}