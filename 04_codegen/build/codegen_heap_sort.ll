; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @is_leq(i32 %left, i32 %right) {
entry:
  %t0 = alloca i32
  store i32 %left, i32* %t0
  %t1 = alloca i32
  store i32 %right, i32* %t1
  br label %while.cond0
while.cond0:
  br label %logic.left3
logic.left3:
  %t2 = load i32, i32* %t0
  %t3 = icmp ne i32 %t2, 0
  br i1 %t3, label %logic.rhs4, label %logic.end5
logic.rhs4:
  %t4 = load i32, i32* %t1
  %t5 = icmp ne i32 %t4, 0
  br label %logic.end5
logic.end5:
  %t6 = phi i1 [0, %logic.left3], [%t5, %logic.rhs4]
  %t7 = zext i1 %t6 to i32
  %t8 = icmp ne i32 %t7, 0
  br i1 %t8, label %while.body1, label %while.end2
while.body1:
  %t9 = load i32, i32* %t0
  %t10 = sub i32 %t9, 1
  store i32 %t10, i32* %t0
  %t11 = load i32, i32* %t1
  %t12 = sub i32 %t11, 1
  store i32 %t12, i32* %t1
  br label %while.cond0
while.end2:
  %t13 = load i32, i32* %t0
  %t14 = icmp ne i32 %t13, 0
  br i1 %t14, label %if.then6, label %if.end7
if.then6:
  ret i32 0
if.end7:
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
define i32 @sift_down(i32* %values, i32 %start, i32 %end) {
entry:
  %t0 = alloca i32
  store i32 %start, i32* %t0
  %t1 = alloca i32
  store i32 0, i32* %t1
  br label %while.cond0
while.cond0:
  %t2 = icmp ne i32 1, 0
  br i1 %t2, label %while.body1, label %while.end2
while.body1:
  %t3 = alloca i32
  %t4 = load i32, i32* %t0
  %t5 = mul i32 %t4, 2
  %t6 = add i32 %t5, 1
  store i32 %t6, i32* %t3
  %t7 = alloca i32
  %t8 = load i32, i32* %t0
  store i32 %t8, i32* %t7
  %t9 = load i32, i32* %t3
  %t10 = call i32 @is_leq(i32 %t9, i32 %end)
  %t11 = icmp eq i32 %t10, 0
  %t12 = zext i1 %t11 to i32
  %t13 = icmp ne i32 %t12, 0
  br i1 %t13, label %if.then3, label %if.end4
if.then3:
  ret i32 0
if.end4:
  %t14 = load i32, i32* %t3
  %t15 = getelementptr i32, i32* %values, i32 %t14
  %t16 = load i32, i32* %t15
  %t17 = load i32, i32* %t7
  %t18 = getelementptr i32, i32* %values, i32 %t17
  %t19 = load i32, i32* %t18
  %t20 = call i32 @is_leq(i32 %t16, i32 %t19)
  %t21 = icmp eq i32 %t20, 0
  %t22 = zext i1 %t21 to i32
  %t23 = icmp ne i32 %t22, 0
  br i1 %t23, label %if.then5, label %if.end6
if.then5:
  %t24 = load i32, i32* %t3
  store i32 %t24, i32* %t7
  br label %if.end6
if.end6:
  %t25 = load i32, i32* %t3
  %t26 = add i32 %t25, 1
  %t27 = call i32 @is_leq(i32 %t26, i32 %end)
  %t28 = icmp ne i32 %t27, 0
  br i1 %t28, label %if.then7, label %if.end8
if.then7:
  %t29 = load i32, i32* %t3
  %t30 = getelementptr i32, i32* %values, i32 %t29
  %t31 = getelementptr i32, i32* %t30, i32 1
  %t32 = load i32, i32* %t31
  %t33 = load i32, i32* %t7
  %t34 = getelementptr i32, i32* %values, i32 %t33
  %t35 = load i32, i32* %t34
  %t36 = call i32 @is_leq(i32 %t32, i32 %t35)
  %t37 = icmp eq i32 %t36, 0
  %t38 = zext i1 %t37 to i32
  %t39 = icmp ne i32 %t38, 0
  br i1 %t39, label %if.then9, label %if.end10
if.then9:
  %t40 = load i32, i32* %t3
  %t41 = add i32 %t40, 1
  store i32 %t41, i32* %t7
  br label %if.end10
if.end10:
  br label %if.end8
if.end8:
  %t42 = load i32, i32* %t7
  %t43 = load i32, i32* %t0
  %t44 = sub i32 %t42, %t43
  %t45 = icmp eq i32 %t44, 0
  %t46 = zext i1 %t45 to i32
  %t47 = icmp ne i32 %t46, 0
  br i1 %t47, label %if.then11, label %if.end12
if.then11:
  ret i32 0
if.end12:
  %t48 = load i32, i32* %t0
  %t49 = getelementptr i32, i32* %values, i32 %t48
  %t50 = load i32, i32* %t7
  %t51 = getelementptr i32, i32* %values, i32 %t50
  %t52 = call i32 @swap_values(i32* %t49, i32* %t51)
  store i32 %t52, i32* %t1
  %t53 = load i32, i32* %t7
  store i32 %t53, i32* %t0
  br label %while.cond0
while.end2:
  ret i32 0
}
define i32 @heap_sort(i32* %values, i32 %count) {
entry:
  %t0 = alloca i32
  store i32 0, i32* %t0
  %t1 = icmp eq i32 %count, 0
  %t2 = zext i1 %t1 to i32
  %t3 = icmp ne i32 %t2, 0
  br i1 %t3, label %if.then0, label %if.end1
if.then0:
  ret i32 0
if.end1:
  %t4 = alloca i32
  %t5 = sub i32 %count, 2
  %t6 = sdiv i32 %t5, 2
  store i32 %t6, i32* %t4
  br label %for.cond2
for.cond2:
  %t7 = load i32, i32* %t4
  %t8 = add i32 %t7, 1
  %t9 = icmp ne i32 %t8, 0
  br i1 %t9, label %for.body3, label %for.end5
for.body3:
  %t10 = load i32, i32* %t4
  %t11 = sub i32 %count, 1
  %t12 = call i32 @sift_down(i32* %values, i32 %t10, i32 %t11)
  store i32 %t12, i32* %t0
  br label %for.inc4
for.inc4:
  %t13 = load i32, i32* %t4
  %t14 = sub i32 %t13, 1
  store i32 %t14, i32* %t4
  br label %for.cond2
for.end5:
  %t15 = alloca i32
  %t16 = sub i32 %count, 1
  store i32 %t16, i32* %t15
  br label %for.cond6
for.cond6:
  %t17 = load i32, i32* %t15
  %t18 = icmp ne i32 %t17, 0
  br i1 %t18, label %for.body7, label %for.end9
for.body7:
  %t19 = load i32, i32* %t15
  %t20 = getelementptr i32, i32* %values, i32 %t19
  %t21 = call i32 @swap_values(i32* %values, i32* %t20)
  store i32 %t21, i32* %t0
  %t22 = load i32, i32* %t15
  %t23 = sub i32 %t22, 1
  %t24 = call i32 @sift_down(i32* %values, i32 0, i32 %t23)
  store i32 %t24, i32* %t0
  br label %for.inc8
for.inc8:
  %t25 = load i32, i32* %t15
  %t26 = sub i32 %t25, 1
  store i32 %t26, i32* %t15
  br label %for.cond6
for.end9:
  ret i32 0
}
