; ModuleID = 'basecc'
source_filename = "basecc"

@global = global [3 x i32] zeroinitializer
define i32 @main() {
entry:
  %t0 = alloca [2 x i32]
  %t1 = getelementptr [3 x i32], [3 x i32]* @global, i32 0, i32 0
  %t2 = getelementptr i32, i32* %t1, i32 0
  store i32 1, i32* %t2
  %t3 = getelementptr [3 x i32], [3 x i32]* @global, i32 0, i32 0
  %t4 = getelementptr i32, i32* %t3, i32 1
  store i32 2, i32* %t4
  %t5 = getelementptr [2 x i32], [2 x i32]* %t0, i32 0, i32 0
  %t6 = getelementptr i32, i32* %t5, i32 0
  %t7 = getelementptr [3 x i32], [3 x i32]* @global, i32 0, i32 0
  %t8 = getelementptr i32, i32* %t7, i32 0
  %t9 = load i32, i32* %t8
  %t10 = getelementptr [3 x i32], [3 x i32]* @global, i32 0, i32 0
  %t11 = getelementptr i32, i32* %t10, i32 1
  %t12 = load i32, i32* %t11
  %t13 = add i32 %t9, %t12
  store i32 %t13, i32* %t6
  %t14 = getelementptr [2 x i32], [2 x i32]* %t0, i32 0, i32 0
  %t15 = getelementptr i32, i32* %t14, i32 1
  store i32 7, i32* %t15
  %t16 = getelementptr [2 x i32], [2 x i32]* %t0, i32 0, i32 0
  %t17 = getelementptr i32, i32* %t16, i32 0
  %t18 = load i32, i32* %t17
  %t19 = getelementptr [2 x i32], [2 x i32]* %t0, i32 0, i32 0
  %t20 = getelementptr i32, i32* %t19, i32 1
  %t21 = load i32, i32* %t20
  %t22 = add i32 %t18, %t21
  ret i32 %t22
}
