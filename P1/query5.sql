SELECT
    o.country,
    COUNT(o.officecode) AS oficinas_sin_ventas
FROM offices AS o
WHERE NOT EXISTS (
    SELECT 1
    FROM employees AS e
    JOIN customers AS c
        ON e.employeenumber = c.salesrepemployeenumber
    JOIN orders AS ord
        ON c.customernumber = ord.customernumber
    WHERE e.officecode = o.officecode
      AND EXTRACT(YEAR FROM ord.orderdate) = 2003
)
GROUP BY
    o.country
ORDER BY
    oficinas_sin_ventas DESC,
    o.country ASC;
