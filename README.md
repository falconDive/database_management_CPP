# AltSQL

AltSQL is a C++ implementation to manage a database using multiple tables linked together with each other

## Compilation

Compile with any C++ generic compiler (eg. g++)

```bash
g++ *.cpp -o AltSQL
```

## Usage

Table can be created by a command like:
```
CREATE TABLE CUSTOMERS (ID INT NOT NULL AUTO_INCREMENT, 
NAME TEXT NOT NULL, 
AGE INT NOT NULL, 
ADDRESS TEXT,
SALARY FLOAT, 
PRIMARY KEY (ID) ); 
```

Tables can be removed by:

```
DROP TABLE CUSTOMERS;
```

In order to insert a single record in the table, the user can use a terminal, a code like this: 

```
INSERT INTO CUSTOMERS (AGE, ADDRESS, NAME) 
VALUES (20, “via Roma 10, Torino”, “Francesco Rossi”); 
```

With the DELETE command, records can can be deleted, such as : 
```
DELETE FROM CUSTOMERS WHERE NAME=”Francesco Rossi”;
```

Table can be emptied by using: 
```bash
TRUNCATE TABLE CUSTOMERS;
```

Tables can be displayed and constrained by commands like :
```
SELECT * FROM CUSTOMERS;
SELECT ID, AGE, SALARY FROM CUSTOMERS;
SELECT ID, AGE, SALARY FROM CUSTOMERS WHERE AGE = 20; 
SELECT ID, AGE, SALARY FROM CUSTOMERS WHERE AGE BETWEEN 20 AND 30;

```

Comparison operators allowed for WHERE operation:
```
 = (EQUALS), 
 > (Greater than), 
 < (Less than), 
 >= (Greater than or equal), 
 <= (Less than or equal), 
 < > Not equal.

```
WHERE can also be used with multiple comparisons. For example
```
SELECT * FROM CUSTOMERS WHERE AGE < 25 AND SALARY > 30000;
```

Tables can be sorted while being displayed by:
```
SELECT ID, AGE, SALARY FROM CUSTOMERS WHERE AGE = 20 
ORDER BY NAME DESC; 

```

Multiple tables can also be linked together. For example:

The foreign table:
```
CREATE TABLE COUNTRIES(
ID INT NOT NULL AUTO_INCREMENT, 
NAME TEXT 	NOT NULL, 
PRIMARY KEY (ID)
); 
```
The table which uses the foreign table:
```
CREATE TABLE CUSTOMERS( 
ID INT NOT NULL AUTO_INCREMENT, 
NAME 	TEXT NOT NULL, 
AGE 	INT	 NOT NULL, 
ADDRESS TEXT , 
COUNTRY_ID INT, 
SALARY FLOAT, 
PRIMARY KEY (ID) ,
FOREIGN KEY (COUNTRY_ID) REFERENCES COUNTRIES (ID) 
);
```


