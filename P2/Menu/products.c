#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "products.h"
#include "odbc.h"

#define BufferLength 512

static int ShowProductsSubMenu(void);

/**
 * @brief Shows the Products submenu and processes user selection.
 *
 *
 * @return selected option
 * @author Alejandro Echeverria
 */
void ShowProductsMenu(SQLHDBC dbc) {
    int nChoice = 0;
    char productCode[BufferLength] = "\0";
    char productName[BufferLength] = "\0";

    do {
        nChoice = ShowProductsSubMenu();
        switch (nChoice) {

            case 1: {
                printf("Enter the product number to check stock > ");
                if (fgets(productCode, (int) sizeof(productCode), stdin)) {
                    /* Eliminamos el salto de línea al final */
                    productCode[strcspn(productCode, "\r\n")] = '\0';
                    (void) ProductStock(dbc, productCode);
                }
            }
                break;

            case 2: {
                printf("Enter the product name to find > ");
                if (fgets(productName, (int) sizeof(productName), stdin)) {
                    /* Eliminamos el salto de línea al final */
                    productName[strcspn(productName, "\r\n")] = '\0';
                    (void) ProductFind(dbc, productName);
                }
            }
                break;

            case 3: {
                printf("Bye Bye\n\n");
            }
                break;

            default:
                break;
        }
    } while (nChoice != 3);
}
/**
 * @brief prints the Products submenu and allows to select an option.
 *
 * @return selected option
 * @author Alejandro Echeverria
 */

int ShowProductsSubMenu() {
    int nSelected = 0;
    char buf[16];

    do {

        printf(" (1) Stock\n"
               " (2) Find\n"
               " (3) Back\n\n");

        printf("Enter a number that corresponds to your choice > ");
        if (!fgets(buf, (int) sizeof(buf), stdin))
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

int ProductStock(SQLHDBC dbc, char *productCode) {

    SQLHSTMT stmt = NULL;

    int ret;
    SQLRETURN ret2;

    SQLINTEGER stock = 0;

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", dbc, SQL_HANDLE_DBC);
        return EXIT_FAILURE;
    }

    /* Prepare statement */
    ret = SQLPrepare(stmt, (SQLCHAR *)
        "SELECT quantityinstock "
        "FROM products "
        "WHERE productcode = ?", SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Bind parameter: productCode como cadena de entrada */
    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                           SQL_C_CHAR, SQL_VARCHAR,
                           strlen(productCode), 0,
                           productCode, 0, NULL);
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

    /* Bind column: stock como entero */
    (void) SQLBindCol(stmt, 1, SQL_C_SLONG, &stock, 0, NULL);

    /* Fetch: solo devuelve 1 fila con las unidades */
    if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%d\n", stock);
    } else {
        /* Si no devuelve fila, el producto no existe en la base de datos */
        printf("Product not found\n");
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

int ProductFind(SQLHDBC dbc, char *name) {

    SQLHSTMT stmt = NULL;

    int ret;
    SQLRETURN ret2;

    #define BufferLength 512

    char x[BufferLength] = "\0";
    char code[BufferLength] = "\0";
    char productName[BufferLength] = "\0";

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
        "SELECT productcode, productname "
        "FROM products "
        "WHERE productname LIKE ? "
        "ORDER BY productcode", SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Bind parameter */
    (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                            SQL_C_CHAR, SQL_VARCHAR,
                            BufferLength, 0, x, BufferLength, NULL);

    /* Execute query */
    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Bind columns */
    (void) SQLBindCol(stmt, 1, SQL_C_CHAR, code, BufferLength, NULL);
    (void) SQLBindCol(stmt, 2, SQL_C_CHAR, productName, BufferLength, NULL);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%s | %s\n", code, productName);
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