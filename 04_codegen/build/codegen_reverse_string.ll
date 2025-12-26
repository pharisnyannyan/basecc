; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @reverse_string(i8* %buffer, i32 %length) {
entry:
  %t0 = alloca i32
  store i32 0, i32* %t0
  %t1 = alloca i32
  %t2 = sub i32 %length, 1
  store i32 %t2, i32* %t1
  %t3 = alloca i32
  %t4 = sdiv i32 %length, 2
  store i32 %t4, i32* %t3
  br label %while.cond0
while.cond0:
  %t5 = load i32, i32* %t3
  %t6 = icmp ne i32 %t5, 0
  br i1 %t6, label %while.body1, label %while.end2
while.body1:
  %t7 = alloca i8
  %t8 = load i32, i32* %t0
  %t9 = getelementptr i8, i8* %buffer, i32 %t8
  %t10 = load i8, i8* %t9
  store i8 %t10, i8* %t7
  %t11 = load i32, i32* %t0
  %t12 = getelementptr i8, i8* %buffer, i32 %t11
  %t13 = load i32, i32* %t1
  %t14 = getelementptr i8, i8* %buffer, i32 %t13
  %t15 = load i8, i8* %t14
  store i8 %t15, i8* %t12
  %t16 = load i32, i32* %t1
  %t17 = getelementptr i8, i8* %buffer, i32 %t16
  %t18 = load i8, i8* %t7
  store i8 %t18, i8* %t17
  %t19 = load i32, i32* %t0
  %t20 = add i32 %t19, 1
  store i32 %t20, i32* %t0
  %t21 = load i32, i32* %t1
  %t22 = sub i32 %t21, 1
  store i32 %t22, i32* %t1
  %t23 = load i32, i32* %t3
  %t24 = sub i32 %t23, 1
  store i32 %t24, i32* %t3
  br label %while.cond0
while.end2:
  ret i32 0
}
