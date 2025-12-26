; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @fib_recursive(i32 %n) {
entry:
  %t0 = icmp ne i32 %n, 0
  br i1 %t0, label %if.then0, label %if.end1
if.then0:
  %t1 = sub i32 %n, 1
  %t2 = icmp ne i32 %t1, 0
  br i1 %t2, label %if.then2, label %if.end3
if.then2:
  %t3 = sub i32 %n, 1
  %t4 = call i32 @fib_recursive(i32 %t3)
  %t5 = sub i32 %n, 2
  %t6 = call i32 @fib_recursive(i32 %t5)
  %t7 = add i32 %t4, %t6
  ret i32 %t7
if.end3:
  br label %if.end1
if.end1:
  ret i32 %n
}
define i32 @fib_iterative(i32 %n) {
entry:
  %t0 = alloca i32
  store i32 0, i32* %t0
  %t1 = alloca i32
  store i32 1, i32* %t1
  %t2 = alloca i32
  store i32 %n, i32* %t2
  %t3 = alloca i32
  store i32 0, i32* %t3
  br label %while.cond0
while.cond0:
  %t4 = load i32, i32* %t2
  %t5 = icmp ne i32 %t4, 0
  br i1 %t5, label %while.body1, label %while.end2
while.body1:
  %t6 = load i32, i32* %t0
  %t7 = load i32, i32* %t1
  %t8 = add i32 %t6, %t7
  store i32 %t8, i32* %t3
  %t9 = load i32, i32* %t1
  store i32 %t9, i32* %t0
  %t10 = load i32, i32* %t3
  store i32 %t10, i32* %t1
  %t11 = load i32, i32* %t2
  %t12 = sub i32 %t11, 1
  store i32 %t12, i32* %t2
  br label %while.cond0
while.end2:
  %t13 = load i32, i32* %t0
  ret i32 %t13
}
