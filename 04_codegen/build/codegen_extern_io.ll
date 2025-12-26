; ModuleID = 'basecc'
source_filename = "basecc"

declare i32 @read(i32, i8*, i32)
declare i32 @write(i32, i8*, i32)
define i32 @extern_io() {
entry:
  %t0 = alloca [5 x i8]
  %t1 = alloca [7 x i8]
  %t2 = alloca [7 x i8]
  %t3 = alloca i32
  %t4 = alloca i32
  %t5 = getelementptr [7 x i8], [7 x i8]* %t1, i32 0, i32 0
  %t6 = getelementptr i8, i8* %t5, i32 0
  %t7 = trunc i32 115 to i8
  store i8 %t7, i8* %t6
  %t8 = getelementptr [7 x i8], [7 x i8]* %t1, i32 0, i32 0
  %t9 = getelementptr i8, i8* %t8, i32 1
  %t10 = trunc i32 116 to i8
  store i8 %t10, i8* %t9
  %t11 = getelementptr [7 x i8], [7 x i8]* %t1, i32 0, i32 0
  %t12 = getelementptr i8, i8* %t11, i32 2
  %t13 = trunc i32 100 to i8
  store i8 %t13, i8* %t12
  %t14 = getelementptr [7 x i8], [7 x i8]* %t1, i32 0, i32 0
  %t15 = getelementptr i8, i8* %t14, i32 3
  %t16 = trunc i32 111 to i8
  store i8 %t16, i8* %t15
  %t17 = getelementptr [7 x i8], [7 x i8]* %t1, i32 0, i32 0
  %t18 = getelementptr i8, i8* %t17, i32 4
  %t19 = trunc i32 117 to i8
  store i8 %t19, i8* %t18
  %t20 = getelementptr [7 x i8], [7 x i8]* %t1, i32 0, i32 0
  %t21 = getelementptr i8, i8* %t20, i32 5
  %t22 = trunc i32 116 to i8
  store i8 %t22, i8* %t21
  %t23 = getelementptr [7 x i8], [7 x i8]* %t1, i32 0, i32 0
  %t24 = getelementptr i8, i8* %t23, i32 6
  %t25 = trunc i32 10 to i8
  store i8 %t25, i8* %t24
  %t26 = getelementptr [7 x i8], [7 x i8]* %t2, i32 0, i32 0
  %t27 = getelementptr i8, i8* %t26, i32 0
  %t28 = trunc i32 115 to i8
  store i8 %t28, i8* %t27
  %t29 = getelementptr [7 x i8], [7 x i8]* %t2, i32 0, i32 0
  %t30 = getelementptr i8, i8* %t29, i32 1
  %t31 = trunc i32 116 to i8
  store i8 %t31, i8* %t30
  %t32 = getelementptr [7 x i8], [7 x i8]* %t2, i32 0, i32 0
  %t33 = getelementptr i8, i8* %t32, i32 2
  %t34 = trunc i32 100 to i8
  store i8 %t34, i8* %t33
  %t35 = getelementptr [7 x i8], [7 x i8]* %t2, i32 0, i32 0
  %t36 = getelementptr i8, i8* %t35, i32 3
  %t37 = trunc i32 101 to i8
  store i8 %t37, i8* %t36
  %t38 = getelementptr [7 x i8], [7 x i8]* %t2, i32 0, i32 0
  %t39 = getelementptr i8, i8* %t38, i32 4
  %t40 = trunc i32 114 to i8
  store i8 %t40, i8* %t39
  %t41 = getelementptr [7 x i8], [7 x i8]* %t2, i32 0, i32 0
  %t42 = getelementptr i8, i8* %t41, i32 5
  %t43 = trunc i32 114 to i8
  store i8 %t43, i8* %t42
  %t44 = getelementptr [7 x i8], [7 x i8]* %t2, i32 0, i32 0
  %t45 = getelementptr i8, i8* %t44, i32 6
  %t46 = trunc i32 10 to i8
  store i8 %t46, i8* %t45
  %t47 = getelementptr [5 x i8], [5 x i8]* %t0, i32 0, i32 0
  %t48 = call i32 @read(i32 0, i8* %t47, i32 5)
  store i32 %t48, i32* %t3
  %t49 = getelementptr [7 x i8], [7 x i8]* %t1, i32 0, i32 0
  %t50 = call i32 @write(i32 1, i8* %t49, i32 7)
  store i32 %t50, i32* %t4
  %t51 = getelementptr [7 x i8], [7 x i8]* %t2, i32 0, i32 0
  %t52 = call i32 @write(i32 2, i8* %t51, i32 7)
  store i32 %t52, i32* %t4
  %t53 = load i32, i32* %t3
  %t54 = icmp ne i32 %t53, 0
  br i1 %t54, label %if.then0, label %if.end1
if.then0:
  %t55 = getelementptr [5 x i8], [5 x i8]* %t0, i32 0, i32 0
  %t56 = load i32, i32* %t3
  %t57 = call i32 @write(i32 1, i8* %t55, i32 %t56)
  store i32 %t57, i32* %t4
  br label %if.end1
if.end1:
  %t58 = load i32, i32* %t3
  %t59 = load i32, i32* %t4
  %t60 = add i32 %t58, %t59
  ret i32 %t60
}
