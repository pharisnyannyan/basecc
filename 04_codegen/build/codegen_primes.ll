; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @fill_primes(i32* %buffer, i32 %max) {
entry:
  %t0 = alloca i32
  store i32 0, i32* %t0
  %t1 = alloca i32
  store i32 2, i32* %t1
  br label %while.cond0
while.cond0:
  %t2 = icmp ne i32 1, 0
  br i1 %t2, label %while.body1, label %while.end2
while.body1:
  %t3 = load i32, i32* %t1
  %t4 = sub i32 %max, %t3
  %t5 = add i32 %t4, 1
  %t6 = icmp eq i32 %t5, 0
  %t7 = zext i1 %t6 to i32
  %t8 = icmp ne i32 %t7, 0
  br i1 %t8, label %if.then3, label %if.end4
if.then3:
  br label %while.end2
if.end4:
  %t9 = alloca i32
  store i32 1, i32* %t9
  %t10 = alloca i32
  store i32 2, i32* %t10
  br label %for.cond5
for.cond5:
  %t11 = load i32, i32* %t1
  %t12 = load i32, i32* %t10
  %t13 = sub i32 %t11, %t12
  %t14 = icmp ne i32 %t13, 0
  br i1 %t14, label %for.body6, label %for.end8
for.body6:
  %t15 = load i32, i32* %t1
  %t16 = load i32, i32* %t10
  %t17 = srem i32 %t15, %t16
  %t18 = icmp eq i32 %t17, 0
  %t19 = zext i1 %t18 to i32
  %t20 = icmp ne i32 %t19, 0
  br i1 %t20, label %if.then9, label %if.end10
if.then9:
  store i32 0, i32* %t9
  br label %if.end10
if.end10:
  br label %for.inc7
for.inc7:
  %t21 = load i32, i32* %t10
  %t22 = add i32 %t21, 1
  store i32 %t22, i32* %t10
  br label %for.cond5
for.end8:
  %t23 = load i32, i32* %t9
  %t24 = icmp ne i32 %t23, 0
  br i1 %t24, label %if.then11, label %if.end12
if.then11:
  %t25 = load i32, i32* %t0
  %t26 = getelementptr i32, i32* %buffer, i32 %t25
  %t27 = load i32, i32* %t1
  store i32 %t27, i32* %t26
  %t28 = load i32, i32* %t0
  %t29 = add i32 %t28, 1
  store i32 %t29, i32* %t0
  br label %if.end12
if.end12:
  %t30 = load i32, i32* %t1
  %t31 = add i32 %t30, 1
  store i32 %t31, i32* %t1
  br label %while.cond0
while.end2:
  %t32 = load i32, i32* %t0
  ret i32 %t32
}
