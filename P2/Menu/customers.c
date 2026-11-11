#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "customers.h"
#include "odbc.h"

#define BufferLength 512

static int ShowCustomersSubMenu(void);

/**
 * @brief Shows the Customers submenu and processes user selection.
 *
 *
 * @return selected option
 * @author Alejandro Echeverria
 */

void ShowCustomersMenu(SQLHDBC dbc) {
    int nChoice = 0;
    char *customername;
    int customernumber;
    

    do {
        nChoice = ShowCustomersSubMenu();
        switch (nChoice) {
            case 1: {
                fprintf(stdin, "Enter the customer name to find > ");
                fscanf(stdin, "%s", customername);
                FindCustomer(dbc, customername);
            }
                break;

            case 2: {
                fprintf(stdin, "Enter the customer number to list products > ");
                fscanf(stdin, "%d", &customernumber);
                ListProducts(dbc, customernumber);
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

/**
 * @brief prints the Customers submenu and allows to select an option.
 *
 * @return selected option
 * @author Alejandro Echeverria
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
/**
 * @brief Opción 1: Find Customers por nombre o apellido de contacto.
 */
int FindCustomer(SQLHDBC dbc, char *name) {

    SQLHSTMT stmt = NULL;

    int ret;
    SQLRETURN ret2;

    char x[BufferLength] = "\0";
    char y[BufferLength] = "\0";
    char z[BufferLength] = "\0";
    char w[BufferLength] = "\0";

    SQLINTEGER n;
    int i = 0;

    /* Add % before and after the name */
    x[0] = '%';

    while (name[i] != '\0' && i < BufferLength - 3) {
        x[i + 1] = name[i];
        i++;
    }

    x[i + 1] = '%';
    x[i + 2] = '\0';

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", dbc, SQL_HANDLE_DBC);
        return EXIT_FAILURE;
    }

    ret = SQLPrepare(stmt, (SQLCHAR *)
        "SELECT customerNumber, customerName, contactFirstName, contactLastName "
        "FROM customers "
        "WHERE contactFirstName LIKE ? OR contactLastName LIKE ? "
        "ORDER BY customerNumber", SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Bind parameters */
    (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, BufferLength, 0, x, BufferLength, NULL);

    (void) SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, BufferLength, 0, x, BufferLength, NULL);

    /* Execute query */
    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Bind columns */
    (void) SQLBindCol(stmt, 1, SQL_C_SLONG, &n, 0, NULL);
    (void) SQLBindCol(stmt, 2, SQL_C_CHAR, y, BufferLength, NULL);
    (void) SQLBindCol(stmt, 3, SQL_C_CHAR, z, BufferLength, NULL);
    (void) SQLBindCol(stmt, 4, SQL_C_CHAR, w, BufferLength, NULL);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%d | %s | %s | %s\n", n, y, z, w);
    }

    if (ret != SQL_NO_DATA) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLCloseCursor(stmt);

    /* Free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


/**
 * @brief Opción 2: List Products comprados por un cliente específico.
 */
void ListProducts(SQLHDBC dbc, SQLINTEGER customernumber)
{
    SQLHENV env = NULL;
    SQLHSTMT stmt = NULL;

    SQLRETURN ret;

    SQLINTEGER x;
    SQLINTEGER offset;
    SQLINTEGER z;

    char y[BufferLength] = "\0";

    SQLLEN indicator1;
    SQLLEN indicator2;
    SQLLEN indicator3;

    int page = 0;
    int i;
    int has_next;
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
        "SELECT p.productCode, p.productName, SUM(od.quantityOrdered) "
        "FROM products p "
        "JOIN orderdetails od ON p.productCode = od.productCode "
        "JOIN orders o ON od.orderNumber = o.orderNumber "
        "WHERE o.customerNumber = ? "
        "GROUP BY p.productCode, p.productName "
        "ORDER BY p.productCode "
        "LIMIT 11 OFFSET ?",
        SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLPrepare", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    /* Bind customer number */
    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
        SQL_C_SLONG, SQL_INTEGER, 0, 0,
        &customernumber, 0, NULL);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindParameter", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    /* Bind offset */
    ret = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT,
        SQL_C_SLONG, SQL_INTEGER, 0, 0,
        &offset, 0, NULL);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindParameter", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    /* Bind result columns */
    ret = SQLBindCol(stmt, 1, SQL_C_SLONG,
        &x, sizeof(x), &indicator1);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindCol", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    ret = SQLBindCol(stmt, 2, SQL_C_CHAR,
        y, sizeof(y), &indicator2);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindCol", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    ret = SQLBindCol(stmt, 3, SQL_C_SLONG,
        &z, sizeof(z), &indicator3);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindCol", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
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
            break;
        }

        printf("\nProducts purchased by customer %d\n", customernumber);
        printf("---------------------------------------------\n");
        printf("%-35s %s\n", "Product name", "Units ordered");
        printf("---------------------------------------------\n");

        /* Display the first 10 products */
        while (i < 10) {

            ret = SQLFetch(stmt);

            if (ret == SQL_NO_DATA) {
                break;
            }

            if (!SQL_SUCCEEDED(ret)) {
                odbc_extract_error("SQLFetch", stmt, SQL_HANDLE_STMT);
                break;
            }

            printf("%-35s %d\n", y, z);

            i++;
        }

        if (!SQL_SUCCEEDED(ret) && ret != SQL_NO_DATA) {
            break;
        }

        /* Check whether an 11th product exists */
        if (i == 10) {

            ret = SQLFetch(stmt);

            if (SQL_SUCCEEDED(ret)) {
                has_next = 1;
            }
            else if (ret != SQL_NO_DATA) {
                odbc_extract_error("SQLFetch", stmt, SQL_HANDLE_STMT);
                break;
            }
        }

        if (i == 0) {
            printf("No products found for this customer.\n");
            SQLCloseCursor(stmt);
            break;
        }

        printf("---------------------------------------------\n");

        if (page > 0) {
            printf("< Previous page");
        }

        if (page > 0 && has_next) {
            printf(" | ");
        }

        if (has_next) {
            printf("> Next page");
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
}

/**
 * @brief Opción 3: Balance total (pagos - compras) del cliente.
 */
int Balance(SQLHDBC dbc, int customerNumber) {

    SQLHSTMT stmt = NULL;

    int ret;
    SQLRETURN ret2;

    double balance = 0.0;
    SQLINTEGER custNum = (SQLINTEGER) customerNumber;

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", dbc, SQL_HANDLE_DBC);
        return EXIT_FAILURE;
    }

    /* Prepare statement */
    ret = SQLPrepare(stmt, (SQLCHAR *)
        "SELECT "
        "  COALESCE((SELECT SUM(amount) FROM payments WHERE customernumber = ?), 0) - "
        "  COALESCE((SELECT SUM(od.quantityordered * od.priceeach) "
        "            FROM orders o "
        "            JOIN orderdetails od ON o.ordernumber = od.ordernumber "
        "            WHERE o.customernumber = ?), 0) AS balance", SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Bind parameters */
    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                           SQL_C_SLONG, SQL_INTEGER,
                           0, 0, &custNum, 0, NULL);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    ret = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT,
                           SQL_C_SLONG, SQL_INTEGER,
                           0, 0, &custNum, 0, NULL);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Execute query */
    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Bind column */
    (void) SQLBindCol(stmt, 1, SQL_C_DOUBLE, &balance, 0, NULL);

    /* Fetch: solo devuelve 1 fila con el saldo total */
    if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("Balance = %.2f\n", balance);
    } else {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLCloseCursor(stmt);

    /* Free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}