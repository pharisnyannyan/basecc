; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @conv1d(i32* %a, i32 %n, i32* %b, i32 %m, i32* %out) {
entry:
  %t0 = alloca i32
  %t1 = add i32 %n, %m
  %t2 = sub i32 %t1, 1
  store i32 %t2, i32* %t0
  %t3 = alloca i32
  store i32 0, i32* %t3
  br label %for.cond0
for.cond0:
  %t4 = load i32, i32* %t0
  %t5 = load i32, i32* %t3
  %t6 = sub i32 %t4, %t5
  %t7 = icmp ne i32 %t6, 0
  br i1 %t7, label %for.body1, label %for.end3
for.body1:
  %t8 = load i32, i32* %t3
  %t9 = getelementptr i32, i32* %out, i32 %t8
  store i32 0, i32* %t9
  br label %for.inc2
for.inc2:
  %t10 = load i32, i32* %t3
  %t11 = add i32 %t10, 1
  store i32 %t11, i32* %t3
  br label %for.cond0
for.end3:
  %t12 = alloca i32
  store i32 0, i32* %t12
  br label %for.cond4
for.cond4:
  %t13 = load i32, i32* %t12
  %t14 = sub i32 %n, %t13
  %t15 = icmp ne i32 %t14, 0
  br i1 %t15, label %for.body5, label %for.end7
for.body5:
  %t16 = alloca i32
  store i32 0, i32* %t16
  br label %for.cond8
for.cond8:
  %t17 = load i32, i32* %t16
  %t18 = sub i32 %m, %t17
  %t19 = icmp ne i32 %t18, 0
  br i1 %t19, label %for.body9, label %for.end11
for.body9:
  %t20 = load i32, i32* %t12
  %t21 = load i32, i32* %t16
  %t22 = add i32 %t20, %t21
  %t23 = getelementptr i32, i32* %out, i32 %t22
  %t24 = load i32, i32* %t12
  %t25 = load i32, i32* %t16
  %t26 = add i32 %t24, %t25
  %t27 = getelementptr i32, i32* %out, i32 %t26
  %t28 = load i32, i32* %t27
  %t29 = load i32, i32* %t12
  %t30 = getelementptr i32, i32* %a, i32 %t29
  %t31 = load i32, i32* %t30
  %t32 = load i32, i32* %t16
  %t33 = getelementptr i32, i32* %b, i32 %t32
  %t34 = load i32, i32* %t33
  %t35 = mul i32 %t31, %t34
  %t36 = add i32 %t28, %t35
  store i32 %t36, i32* %t23
  br label %for.inc10
for.inc10:
  %t37 = load i32, i32* %t16
  %t38 = add i32 %t37, 1
  store i32 %t38, i32* %t16
  br label %for.cond8
for.end11:
  br label %for.inc6
for.inc6:
  %t39 = load i32, i32* %t12
  %t40 = add i32 %t39, 1
  store i32 %t40, i32* %t12
  br label %for.cond4
for.end7:
  ret i32 0
}
