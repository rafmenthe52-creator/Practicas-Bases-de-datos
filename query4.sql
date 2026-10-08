WITH ventas_por_oficina AS (
    SELECT
        e.officecode,
        SUM(od.quantityordered) AS productos_vendidos
    FROM employees AS e
    JOIN customers AS c
        ON c.salesrepemployeenumber = e.employeenumber
    JOIN orders AS o
        ON o.customernumber = c.customernumber
    JOIN orderdetails AS od
        ON od.ordernumber = o.ordernumber
    GROUP BY e.officecode
)
SELECT
    officecode,
    productos_vendidos
FROM ventas_por_oficina
WHERE productos_vendidos = (
    SELECT MAX(productos_vendidos)
    FROM ventas_por_oficina
);

	
	
	
	