; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @sieve_primes(i32* %buffer, i32 %max) {
entry:
  %t0 = alloca [128 x i32]
  %t1 = alloca i32
  store i32 0, i32* %t1
  %t2 = alloca i32
  store i32 0, i32* %t2
  store i32 0, i32* %t1
  br label %for.cond0
for.cond0:
  %t3 = load i32, i32* %t1
  %t4 = sub i32 %max, %t3
  %t5 = add i32 %t4, 1
  %t6 = icmp ne i32 %t5, 0
  br i1 %t6, label %for.body1, label %for.end3
for.body1:
  %t7 = getelementptr [128 x i32], [128 x i32]* %t0, i32 0, i32 0
  %t8 = load i32, i32* %t1
  %t9 = getelementptr i32, i32* %t7, i32 %t8
  store i32 1, i32* %t9
  br label %for.inc2
for.inc2:
  %t10 = load i32, i32* %t1
  %t11 = add i32 %t10, 1
  store i32 %t11, i32* %t1
  br label %for.cond0
for.end3:
  %t12 = getelementptr [128 x i32], [128 x i32]* %t0, i32 0, i32 0
  %t13 = getelementptr i32, i32* %t12, i32 0
  store i32 0, i32* %t13
  %t14 = getelementptr [128 x i32], [128 x i32]* %t0, i32 0, i32 0
  %t15 = getelementptr i32, i32* %t14, i32 1
  store i32 0, i32* %t15
  %t16 = alloca i32
  store i32 2, i32* %t16
  br label %for.cond4
for.cond4:
  %t17 = load i32, i32* %t16
  %t18 = sub i32 %t17, 8
  %t19 = icmp ne i32 %t18, 0
  br i1 %t19, label %for.body5, label %for.end7
for.body5:
  %t20 = getelementptr [128 x i32], [128 x i32]* %t0, i32 0, i32 0
  %t21 = load i32, i32* %t16
  %t22 = getelementptr i32, i32* %t20, i32 %t21
  %t23 = load i32, i32* %t22
  %t24 = icmp ne i32 %t23, 0
  br i1 %t24, label %if.then8, label %if.end9
if.then8:
  %t25 = alloca i32
  %t26 = load i32, i32* %t16
  %t27 = load i32, i32* %t16
  %t28 = add i32 %t26, %t27
  store i32 %t28, i32* %t25
  br label %for.cond10
for.cond10:
  %t29 = load i32, i32* %t25
  %t30 = sub i32 %max, %t29
  %t31 = add i32 %t30, 1
  %t32 = icmp ne i32 %t31, 0
  br i1 %t32, label %for.body11, label %for.end13
for.body11:
  %t33 = load i32, i32* %t25
  %t34 = load i32, i32* %t16
  %t35 = srem i32 %t33, %t34
  %t36 = icmp eq i32 %t35, 0
  %t37 = zext i1 %t36 to i32
  %t38 = icmp ne i32 %t37, 0
  br i1 %t38, label %if.then14, label %if.end15
if.then14:
  %t39 = getelementptr [128 x i32], [128 x i32]* %t0, i32 0, i32 0
  %t40 = load i32, i32* %t25
  %t41 = getelementptr i32, i32* %t39, i32 %t40
  store i32 0, i32* %t41
  br label %if.end15
if.end15:
  br label %for.inc12
for.inc12:
  %t42 = load i32, i32* %t25
  %t43 = add i32 %t42, 1
  store i32 %t43, i32* %t25
  br label %for.cond10
for.end13:
  br label %if.end9
if.end9:
  br label %for.inc6
for.inc6:
  %t44 = load i32, i32* %t16
  %t45 = add i32 %t44, 1
  store i32 %t45, i32* %t16
  br label %for.cond4
for.end7:
  store i32 2, i32* %t1
  br label %for.cond16
for.cond16:
  %t46 = load i32, i32* %t1
  %t47 = sub i32 %max, %t46
  %t48 = add i32 %t47, 1
  %t49 = icmp ne i32 %t48, 0
  br i1 %t49, label %for.body17, label %for.end19
for.body17:
  %t50 = getelementptr [128 x i32], [128 x i32]* %t0, i32 0, i32 0
  %t51 = load i32, i32* %t1
  %t52 = getelementptr i32, i32* %t50, i32 %t51
  %t53 = load i32, i32* %t52
  %t54 = icmp ne i32 %t53, 0
  br i1 %t54, label %if.then20, label %if.end21
if.then20:
  %t55 = load i32, i32* %t2
  %t56 = getelementptr i32, i32* %buffer, i32 %t55
  %t57 = load i32, i32* %t1
  store i32 %t57, i32* %t56
  %t58 = load i32, i32* %t2
  %t59 = add i32 %t58, 1
  store i32 %t59, i32* %t2
  br label %if.end21
if.end21:
  br label %for.inc18
for.inc18:
  %t60 = load i32, i32* %t1
  %t61 = add i32 %t60, 1
  store i32 %t61, i32* %t1
  br label %for.cond16
for.end19:
  %t62 = load i32, i32* %t2
  ret i32 %t62
}
