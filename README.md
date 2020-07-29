Design a database, consisting of a set of pseudo tables that through different commands can be filled, modified, deleted etc...
As far as the table is concerned, there must not be an effective table with the respective columns and rows and therefore consequently the mySQL library MUST NOT be used, but since it is a project that is suitable for the use of classes, lists, trees, templates, these must be used. ..
We need to "invent" a way to act as a database.
that is, I either from the command line or through a file will give commands to my project of the type create #object, delete #object etc ..
If I work with a file, inside it there will be (I repeat) many instructions of the type create "something" that give me the command to create a new pseudo-table.
In this file there will also be commands that want to create the same "OBJECT TYPE" and in this case they will go and be placed in the same column of the same type.
In the meantime that I read these instructions from a file I will go to fill my pseudo table in an orderly way with which you will work in memory (and not from file).
Then there will be another file in which there are table modification instructions, that is if I want to delete a record or an entire table or add an extension to it, and when I find the QUIT () command in this file, I will have to save all the changes of this database on a file and once I start updating it, I will resume from the same point where I left it.
THIS FILE WILL NOT SEE A REAL SET OF TABLES IN WHICH THERE ARE LINES AND COLUMNS BUT YOU NEED TO INVENT A METHOD TO "SEPARATE" TABLE FROM TABLE, FOR EXAMPLE THROUGH SPECIAL CHARACTERS SO AS A TIME THAT I READ THIS TIME I UNDERSTAND THAT IT IS A DIFFERENT TABLE.
Furthermore, it is not necessary to work directly on the files, but they are only used to take the initial data, to subsequently save all the updated data and it will be necessary to create test files through which the correct functioning of my program will be verified.
I advise you to CAREFULLY reread the file that I sent initially because all the steps that must be implemented in the program are explained there.
The input files, where I write the various commands to be executed, are only used to simulate the project.

PLEASE READ THE ATTACHED FILE


Just like data structure definition , you define 

INSERT CUSTOMER 

INSERT INTO CUSTOMER (COLUMN NAME 1 , COLUMN NAME 2) VALUES (VALUE 1 , VALUE 2)
INSERT INTO CUSTOMER (ID , NAME) VALUES (1 , 'A')


That is 
Ipb:: named_vector should act just the way 
Std::vector 
Does
We’re required to write it on our own and call it by ‘ipb‘ name
Similarly for vector


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


