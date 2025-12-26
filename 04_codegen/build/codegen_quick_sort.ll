; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @is_leq(i32 %value, i32 %pivot) {
entry:
  %t0 = icmp ne i32 %pivot, 0
  br i1 %t0, label %if.then0, label %if.end1
if.then0:
  ret i32 1
if.end1:
  %t1 = icmp ne i32 %value, 0
  br i1 %t1, label %if.then2, label %if.end3
if.then2:
  ret i32 0
if.end3:
  ret i32 1
}
define i32 @swap_values(i32* %left, i32* %right) {
entry:
  %t0 = alloca i32
  %t1 = load i32, i32* %left
  store i32 %t1, i32* %t0
  %t2 = load i32, i32* %right
  store i32 %t2, i32* %left
  %t3 = load i32, i32* %t0
  store i32 %t3, i32* %right
  ret i32 0
}
define i32 @partition(i32* %values, i32 %low, i32 %high) {
entry:
  %t0 = alloca i32
  %t1 = getelementptr i32, i32* %values, i32 %high
  %t2 = load i32, i32* %t1
  store i32 %t2, i32* %t0
  %t3 = alloca i32
  %t4 = sub i32 %low, 1
  store i32 %t4, i32* %t3
  %t5 = alloca i32
  store i32 %low, i32* %t5
  %t6 = alloca i32
  %t7 = sub i32 %high, %low
  store i32 %t7, i32* %t6
  %t8 = alloca i32
  store i32 0, i32* %t8
  br label %while.cond0
while.cond0:
  %t9 = load i32, i32* %t6
  %t10 = icmp ne i32 %t9, 0
  br i1 %t10, label %while.body1, label %while.end2
while.body1:
  %t11 = load i32, i32* %t5
  %t12 = getelementptr i32, i32* %values, i32 %t11
  %t13 = load i32, i32* %t12
  %t14 = load i32, i32* %t0
  %t15 = call i32 @is_leq(i32 %t13, i32 %t14)
  %t16 = icmp ne i32 %t15, 0
  br i1 %t16, label %if.then3, label %if.end4
if.then3:
  %t17 = load i32, i32* %t3
  %t18 = add i32 %t17, 1
  store i32 %t18, i32* %t3
  %t19 = load i32, i32* %t3
  %t20 = getelementptr i32, i32* %values, i32 %t19
  %t21 = load i32, i32* %t5
  %t22 = getelementptr i32, i32* %values, i32 %t21
  %t23 = call i32 @swap_values(i32* %t20, i32* %t22)
  store i32 %t23, i32* %t8
  br label %if.end4
if.end4:
  %t24 = load i32, i32* %t5
  %t25 = add i32 %t24, 1
  store i32 %t25, i32* %t5
  %t26 = load i32, i32* %t6
  %t27 = sub i32 %t26, 1
  store i32 %t27, i32* %t6
  br label %while.cond0
while.end2:
  %t28 = load i32, i32* %t3
  %t29 = getelementptr i32, i32* %values, i32 %t28
  %t30 = getelementptr i32, i32* %t29, i32 1
  %t31 = getelementptr i32, i32* %values, i32 %high
  %t32 = call i32 @swap_values(i32* %t30, i32* %t31)
  store i32 %t32, i32* %t8
  %t33 = load i32, i32* %t3
  %t34 = add i32 %t33, 1
  ret i32 %t34
}
define i32 @quick_sort(i32* %values, i32 %low, i32 %high) {
entry:
  %t0 = alloca i32
  store i32 0, i32* %t0
  %t1 = alloca i32
  store i32 0, i32* %t1
  %t2 = alloca i32
  store i32 0, i32* %t2
  %t3 = alloca i32
  store i32 0, i32* %t3
  %t4 = sub i32 %high, %low
  %t5 = icmp eq i32 %t4, 0
  %t6 = zext i1 %t5 to i32
  %t7 = icmp ne i32 %t6, 0
  br i1 %t7, label %if.then0, label %if.end1
if.then0:
  ret i32 0
if.end1:
  %t8 = call i32 @partition(i32* %values, i32 %low, i32 %high)
  store i32 %t8, i32* %t0
  %t9 = load i32, i32* %t0
  %t10 = sub i32 %t9, %low
  store i32 %t10, i32* %t1
  %t11 = load i32, i32* %t1
  %t12 = icmp ne i32 %t11, 0
  br i1 %t12, label %if.then2, label %if.end3
if.then2:
  %t13 = load i32, i32* %t0
  %t14 = sub i32 %t13, 1
  %t15 = call i32 @quick_sort(i32* %values, i32 %low, i32 %t14)
  store i32 %t15, i32* %t3
  br label %if.end3
if.end3:
  %t16 = load i32, i32* %t0
  %t17 = sub i32 %high, %t16
  store i32 %t17, i32* %t2
  %t18 = load i32, i32* %t2
  %t19 = icmp ne i32 %t18, 0
  br i1 %t19, label %if.then4, label %if.end5
if.then4:
  %t20 = load i32, i32* %t0
  %t21 = add i32 %t20, 1
  %t22 = call i32 @quick_sort(i32* %values, i32 %t21, i32 %high)
  store i32 %t22, i32* %t3
  br label %if.end5
if.end5:
  ret i32 0
}
