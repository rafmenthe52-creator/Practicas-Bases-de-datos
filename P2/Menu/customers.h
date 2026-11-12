#ifndef CUSTOMERS_H
#define CUSTOMERS_H

#include <sql.h>
#include <sqlext.h>

/**
 * @brief Muestra el submenú de Customers y procesa la selección del usuario.
 * @param dbc Manejador de la conexión activa a la base de datos.
 * @author Alejandro Echeverria
 */
void ShowCustomersMenu(SQLHDBC dbc);

/**
 * @brief Solicita un nombre/apellido de contacto y lista los clientes coincidentes.
 * @param dbc Manejador de la conexión activa.
 * @author Alejandro Echeverria
 * @return EXIT_SUCCESS o EXIT_FAILURE.
 */
int FindCustomer(SQLHDBC dbc, char *name);

/**
 * @brief Solicita el ID de un cliente y lista los productos comprados y unidades totales.
 * @param dbc Manejador de la conexión activa.
 * @author Alejandro Echeverria
 * @return EXIT_SUCCESS o EXIT_FAILURE.
 */
int ListProducts(SQLHDBC dbc, SQLINTEGER customerNumber);

/**
 * @brief Solicita el ID de un cliente y calcula su saldo (pagos - compras).
 * @param dbc Manejador de la conexión activa.
 * @author Alejandro Echeverria
 * @return EXIT_SUCCESS o EXIT_FAILURE.
 */
int Balance(SQLHDBC dbc, int customerNumber);

#endif

