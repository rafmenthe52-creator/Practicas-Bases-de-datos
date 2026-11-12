#ifndef ORDERS_H
#define ORDERS_H

#include <sql.h>
#include <sqlext.h>

/**
 * @brief Muestra el submenú de Orders y procesa la selección del usuario.
 * @param dbc Manejador de la conexión activa a la base de datos.
 * @author Rafael Velasco Wagner
 */
void ShowOrdersMenu(SQLHDBC dbc);

/**
 * @brief Lista los pedidos abiertos (sin fecha de envío) ordenados por orderNumber.
 * @param dbc Manejador de la conexión activa a la base de datos.
 */
void ListOpenOrders(SQLHDBC dbc);

/**
 * @brief Lista los pedidos realizados entre dos fechas dadas.
 * @param dbc Manejador de la conexión activa a la base de datos.
 * @param start_date Fecha inicial en formato YYYY-MM-DD.
 * @param end_date Fecha final en formato YYYY-MM-DD.
 */
void ListOrdersByRange(SQLHDBC dbc, char *start_date, char *end_date);

/**
 * @brief Muestra la fecha, estado, coste total y líneas de producto de un pedido.
 * @param dbc Manejador de la conexión activa a la base de datos.
 * @param ordernumber Identificador del pedido.
 */
void ShowOrderDetails(SQLHDBC dbc, SQLINTEGER ordernumber);

#endif