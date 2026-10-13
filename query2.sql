SELECT
    t.productline,
    AVG(t.shippeddate - t.orderdate) AS tiempo_medio_dias
FROM (
    SELECT DISTINCT
        o.ordernumber,
        o.orderdate,
        o.shippeddate,
        p.productline
    FROM orders AS o
    JOIN orderdetails AS od
        ON o.ordernumber = od.ordernumber
    JOIN products AS p
        ON od.productcode = p.productcode
    WHERE o.shippeddate IS NOT NULL
) AS t
GROUP BY
    t.productline
ORDER BY
    t.productline;
