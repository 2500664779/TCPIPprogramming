## 2. MySQL简介
### 2.1 什么是MySQL
> * MySQL是一种DBMS，即它是一种数据库软件。 
> * —MySQL是开放源代码的，一般可以免费使用
> * MySQL执行很快

## 3. 使用MySQL
### 3.2 选择数据库
> `USE xxxx`选择某一个数据库
### 3.3 了解数据库
> `SHOW DATABASES`:返回数据库可用的一个表.
> `SHOW TABLES`:获得(当前选中)数据库内的表的列表.
> `SHOW`也可以显示表列.如`SHOW COLUMS FROM xxx`
> `自动增量`:某些表需要唯一值.如订单编号,ID的,*当添加每一行时,MySQL可以自动为每个行分配下一个可用编号*,如果需要则需要在CREATE时添加某些固定部分.
> `SHOW STATUS`:显示服务器状态信息
> `SHOW CREATE DATABASES`和`SHOW CREATE TABLE`,显示创建特定数据库或表.
> `SHOW GRANTS`显示用户的安全权限
> `SHOW ERRORS`和`SHOW WARNINGS`显示错误和警告信息.

## 4. 检索数据
`SELECT colname FROM table`,检索一个名为`colname`的列,从`table`中.

**使用分号来表示命令结束.**
`SELECT colname,xxy FROM table`:**检索多个列.**
`SELECT * FROM table`**使用通配符**.所有的列.
 
**检索不同的行**
`SELECT DISTINCT colname FROM table`检索不同的行.(即colname中只有不同的数据才会被返回.)
**不能部分使用`DISTINCT`**

**限制结果**
`SELECT colname FROM table LIMIT 5`:返回不多于5行.
`SELECT colname FROM table LIMIT 5,5`:第一个5表示从第五行开始,第二个5表示最多5行.**行号为0开始**

**使用完全限定的表名**
`SELECT table.colname FROM table`
`SELECT table.colname FROM database.table`

## 5. 数据检索排序
`SELECT colname FROM table ORDER BY colname`:*可以选择非选择的列进行排序*即`SELECT`没出现的`colname`进行排序

**多个列排序**
`SELECT colname1,colname2,colname3 FROM table ORDER BY colname1,colname3`

**指定排序方向**
`SELECT colname1,colname2,colname3 FROM table ORDER BY colname1 DESC`**DESC关键字只应用到直接位于其前面的列名。**
例如:`SELECT colname1,colname2,colname3 FROM table ORDER BY colname1 DESC, colname2`:即如果多个指定顺序需要在多个后面指定`DESC`,**ASC通常不使用,因为默认是升序的**

**使用`ORDER`和`LIMIT`能选出最大值和最小值**
`SELECT price FROM table ORDER BY price DESC LIMIT 1;`

## 6. 过滤数据
`WHERE`词在表名之后给出,即在`FROM`之后给出.
`SELECT colname1,colname2 FROM table WHERE colname2 = 30;`:返回`colname2 = 30`的记录

**WHERE子句的位置  在同时使用ORDER BY和WHERE子句时，应 该让ORDER BY位于WHERE之后，否则将会产生错误**
![](pictures/WHERE操作符.jpg)
**英文字母默认不区分大小写**字符串限定需要加引号,值比较不需要.例如:`WHERE colname1 = 'xxx'`和`WHERE colname2 = 3`;

**范围值检查**
`SELECT colname1, colname2 FROM table WHERE colname2 BETWEEN 5 AND 10;`

**空值检查**
`SELECT colname1 FROM table WHERE colname2 IS NULL;`

## 7. 组合过滤
**`AND`操作符**
`SELECT colname1, colname2, colname3 FROM table WHERE colname1 = 10 AND colname2 <=49;`
**`OR`操作符**

**`WHERE`可以包含任意次`AND`和`OR`**
但是`AND`优先级高于`OR`可能导致非预期的结果.
`SELECT name, price FROM table WHERE id = 100 OR id = 101 AND price >= 10;`事实结果为:`id = 100 OR (id = 101 AND price >= 10)`
因此需要用括号进行精准限定.

**`IN`操作符**
`SELECT name, price FROM table WHERE id IN (100, 110) AND price >= 10;`**清单必须在`IN`之后的圆括号中**
**优势**:
>* 在使用长的合法选项清单时，IN操作符的语法更清楚且更直观. 
>* 在使用IN时，计算的次序更容易管理（因为使用的操作符更少）。
>* IN操作符一般比OR操作符清单执行更快。 
>* IN的最大优点是可以包含其他SELECT语句，使得能够更动态地建立WHERE子句

**`NOT`操作符**
**否定之后跟条件的关键字.**
`SELECT name, price FROM table WHERE id NOT IN (100, 110) AND price >= 10;`表明要选择不在`100`和`110`中.

## 8. 通配符
**百分号通配符**
`%`表示任何字符出现任意次.
`SELECT name1, name2 FROM table WHERE name2 LIKE 'Jet%';`查询`name2`为任意`jet`开头的行
**通配符可以同时出现在多处**,但 **`%`不能统配NULL**

**下划线`_`**
**匹配单个任意字符**
`SELECT name1, name2 FROM table WHERE name2 LIKE '_ Jet%';`

## 9. 正则表达式
MySQL仅支持正则表达式中的很小一个子集.
`SELECT name1 FROM table WHERE name1 