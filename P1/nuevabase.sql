-- -----------------------------------------------------------------------------
-- Script de creación para el rediseño de la base de datos (nuevabase.sql)
-- Gestor: PostgreSQL
-- -----------------------------------------------------------------------------

-- 1. Tabla offices
CREATE TABLE offices (
    officecode VARCHAR(10) NOT NULL,
    city VARCHAR(50) NOT NULL,
    phone VARCHAR(50) NOT NULL,
    addressline1 VARCHAR(50) NOT NULL,
    addressline2 VARCHAR(50),
    state VARCHAR(50),
    country VARCHAR(50) NOT NULL,
    postalcode VARCHAR(15) NOT NULL,
    territory VARCHAR(10) NOT NULL,
    CONSTRAINT offices_pkey PRIMARY KEY (officecode)
);

-- 2. Tabla employees (Sin officecode; reportsto permite NULL)
CREATE TABLE employees (
    employeenumber INT NOT NULL,
    lastname VARCHAR(50) NOT NULL,
    firstname VARCHAR(50) NOT NULL,
    extension VARCHAR(10) NOT NULL,
    email VARCHAR(100) NOT NULL,
    reportsto INT,
    jobtitle VARCHAR(50) NOT NULL,
    CONSTRAINT employees_pkey PRIMARY KEY (employeenumber),
    CONSTRAINT employees_reportsto_fkey FOREIGN KEY (reportsto) 
        REFERENCES employees(employeenumber) 
);

-- 3. NUEVA TABLA: employee_offices (Histórico de oficinas)
CREATE TABLE employee_offices (
    employeenumber INT NOT NULL,
    officecode VARCHAR(10) NOT NULL,
    startdate DATE NOT NULL,
    enddate DATE,
    CONSTRAINT employee_offices_pkey PRIMARY KEY (employeenumber, officecode, startdate),
    CONSTRAINT emp_off_employee_fkey FOREIGN KEY (employeenumber) 
        REFERENCES employees(employeenumber),
    CONSTRAINT emp_off_office_fkey FOREIGN KEY (officecode) 
        REFERENCES offices(officecode)
);

-- 4. Tabla customers (Sin salesrepemployeenumber)
CREATE TABLE customers (
    customernumber INT NOT NULL,
    customername VARCHAR(50) NOT NULL,
    contactlastname VARCHAR(50) NOT NULL,
    contactfirstname VARCHAR(50) NOT NULL,
    phone VARCHAR(50) NOT NULL,
    addressline1 VARCHAR(50) NOT NULL,
    addressline2 VARCHAR(50),
    city VARCHAR(50) NOT NULL,
    state VARCHAR(50),
    postalcode VARCHAR(15),
    country VARCHAR(50) NOT NULL,
    creditlimit NUMERIC(10,2),
    CONSTRAINT customers_pkey PRIMARY KEY (customernumber)
);

-- 5. NUEVA TABLA: customer_employees (Relación entre Clientes y Comerciales)
CREATE TABLE customer_employees (
    customernumber INT NOT NULL,
    employeenumber INT NOT NULL,
    CONSTRAINT customer_employees_pkey PRIMARY KEY (customernumber, employeenumber),
    CONSTRAINT cust_emp_customer_fkey FOREIGN KEY (customernumber) 
        REFERENCES customers(customernumber),
    CONSTRAINT cust_emp_employee_fkey FOREIGN KEY (employeenumber) 
        REFERENCES employees(employeenumber)
);

-- 6. Tabla productlines
CREATE TABLE productlines (
    productline VARCHAR(50) NOT NULL,
    textdescription VARCHAR(4000),
    htmldescription TEXT,
    image BYTEA,
    CONSTRAINT productlines_pkey PRIMARY KEY (productline)
);

-- 7. Tabla products
CREATE TABLE products (
    productcode VARCHAR(15) NOT NULL,
    productname VARCHAR(70) NOT NULL,
    productline VARCHAR(50) NOT NULL,
    productscale VARCHAR(10) NOT NULL,
    productvendor VARCHAR(50) NOT NULL,
    productdescription TEXT NOT NULL,
    quantityinstock SMALLINT NOT NULL,
    buyprice NUMERIC(10,2) NOT NULL,
    msrp NUMERIC(10,2) NOT NULL,
    CONSTRAINT products_pkey PRIMARY KEY (productcode),
    CONSTRAINT products_productline_fkey FOREIGN KEY (productline) 
        REFERENCES productlines(productline)
);

-- 8. Tabla orders
CREATE TABLE orders (
    ordernumber INT NOT NULL,
    orderdate DATE NOT NULL,
    requireddate DATE NOT NULL,
    shippeddate DATE,
    status VARCHAR(15) NOT NULL,
    comments TEXT,
    customernumber INT NOT NULL,
    CONSTRAINT orders_pkey PRIMARY KEY (ordernumber),
    CONSTRAINT orders_customer_fkey FOREIGN KEY (customernumber) 
        REFERENCES customers(customernumber)
);

-- 9. Tabla orderdetails
CREATE TABLE orderdetails (
    ordernumber INT NOT NULL,
    productcode VARCHAR(15) NOT NULL,
    quantityordered INT NOT NULL,
    priceeach NUMERIC(10,2) NOT NULL,
    orderlinenumber SMALLINT NOT NULL,
    CONSTRAINT orderdetails_pkey PRIMARY KEY (ordernumber, productcode),
    CONSTRAINT details_order_fkey FOREIGN KEY (ordernumber) 
        REFERENCES orders(ordernumber),
    CONSTRAINT details_product_fkey FOREIGN KEY (productcode) 
        REFERENCES products(productcode)
);

-- 10. Tabla payments (Vinculada a orders)
CREATE TABLE payments (
    customernumber INT NOT NULL,
    checknumber VARCHAR(50) NOT NULL,
    ordernumber INT,
    paymentdate DATE NOT NULL,
    amount NUMERIC(10,2) NOT NULL,
    CONSTRAINT payments_pkey PRIMARY KEY (customernumber, checknumber),
    CONSTRAINT payments_customer_fkey FOREIGN KEY (customernumber) 
        REFERENCES customers(customernumber),
    CONSTRAINT payments_order_fkey FOREIGN KEY (ordernumber) 
        REFERENCES orders(ordernumber) 
);