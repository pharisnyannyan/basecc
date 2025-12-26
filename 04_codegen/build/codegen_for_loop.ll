; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @sum_down() {
entry:
  %t0 = alloca i32
  store i32 0, i32* %t0
  %t1 = alloca i32
  store i32 5, i32* %t1
  br label %for.cond0
for.cond0:
  %t2 = load i32, i32* %t1
  %t3 = icmp ne i32 %t2, 0
  br i1 %t3, label %for.body1, label %for.end3
for.body1:
  %t4 = load i32, i32* %t0
  %t5 = load i32, i32* %t1
  %t6 = add i32 %t4, %t5
  store i32 %t6, i32* %t0
  br label %for.inc2
for.inc2:
  %t7 = load i32, i32* %t1
  %t8 = sub i32 %t7, 1
  store i32 %t8, i32* %t1
  br label %for.cond0
for.end3:
  %t9 = load i32, i32* %t0
  ret i32 %t9
}
