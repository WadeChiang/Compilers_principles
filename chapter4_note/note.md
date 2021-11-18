## LL(1)分析

对文法G的每个非终结符A以及其任意候选式$\alpha$，构造$FIRST(\alpha),FOLLOW(A)$。

如果有产生式$A\rightarrow\alpha$，当A在分析栈栈顶时：

1.如果当前符号$a\in FIRST(\alpha)$时，应该用$\alpha$展开A，即表项$M[A,a]$中应该为$A\rightarrow\alpha$

2.如果$\varepsilon\in FIRST(\alpha)$，并且当前输入符号$b\in FOLLOW(A),where b\in V_T\cup\{\$\}$，则$A\rightarrow\alpha$就认为A自动得到了匹配，应把$A\rightarrow\alpha$放入表项$M[A,a]$中。

## SLR(1)分析

**定义4.10**:

LR(0)有效项目：如果存在规范推导$S\Rightarrow \delta A\omega\Rightarrow \delta\beta_1\beta_2\omega$，那么LR(0)项目$A\rightarrow\beta_1\cdot\beta_2$对活前缀$\gamma=\delta\beta_1$是有效的.

**推广定义**：

If LR(0)项目$A\rightarrow \alpha\cdot\beta$对活前缀$\gamma=\delta\alpha$是有效的，这里$B\in V_N$并且$B\rightarrow\eta$是产生式，那么LR(0) 项目$B\rightarrow\cdot \eta$对活前缀$ \gamma=\delta\alpha$也是有效的。

$\gamma$的LR(0)有效项目集：{文法G的某个活前缀$\gamma$的所有LR(0)有效项目}

G的LR(0)项目集规范族：{G的所有活前缀的LR(0)有效项目集}

**构造LR(0)项目集规范族**：

![image-20211031143941619](C:\Users\WyrdE\AppData\Roaming\Typora\typora-user-images\image-20211031143941619.png)

![image-20211031151154401](C:\Users\WyrdE\AppData\Roaming\Typora\typora-user-images\image-20211031151154401.png)

![image-20211031144916509](C:\Users\WyrdE\AppData\Roaming\Typora\typora-user-images\image-20211031144916509.png)

## LR(1)分析

**定义4.13**:

LR(1)有效项目：如果存在规范推导$S\Rightarrow \delta A\omega\Rightarrow \delta\alpha\beta\omega$，那么LR(1)项目$[A\rightarrow\alpha\cdot\beta,a]$对活前缀$\gamma=\delta\alpha$是有效的，其中$a\in FIRST(\omega\$)$

**推广定义**：

如果LR(1)项目$[A\rightarrow\alpha\cdot B\beta,a]$对活前缀$\gamma=\delta\alpha$是有效的，这里$B\in V_N$并且$B\rightarrow\eta$是产生式，那么对任意$b\in FIRST(\beta a)$，LR(1)项目$[B\rightarrow\cdot\eta,b]$对活前缀$\gamma=\delta\alpha$是有效的

$\gamma$的LR(1)有效项目集：{文法G的某个活前缀$\gamma$的所有LR(1)有效项目}

G的LR(1)项目集规范族：{G的所有活前缀的LR(1)有效项目集}

**构造LR(1)项目集规范族**：

![image-20211031100727852](C:\Users\WyrdE\AppData\Roaming\Typora\typora-user-images\image-20211031100727852.png)

**go函数**：

![image-20211031101059250](C:\Users\WyrdE\AppData\Roaming\Typora\typora-user-images\image-20211031101059250.png)

![image-20211031101215081](C:\Users\WyrdE\AppData\Roaming\Typora\typora-user-images\image-20211031101215081.png)

构造项目集规范族时判断状态是否已经存在可以从闭包入手，比较产生该状态的闭包比比较状态本身要快得多。

## LALR(1)分析

