SELECT
    e.employeenumber,
    e.lastname
FROM employees AS e
JOIN employees AS m
    ON e.reportsto = m.employeenumber
JOIN employees AS d
    ON m.reportsto = d.employeenumber
WHERE d.reportsto IS NULL;
