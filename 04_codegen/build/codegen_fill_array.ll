; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @fill_array(i32* %buffer, i32 %n) {
entry:
  %t0 = alloca i32
  store i32 0, i32* %t0
  br label %for.cond0
for.cond0:
  %t1 = load i32, i32* %t0
  %t2 = sub i32 %n, %t1
  %t3 = icmp ne i32 %t2, 0
  br i1 %t3, label %for.body1, label %for.end3
for.body1:
  %t4 = load i32, i32* %t0
  %t5 = getelementptr i32, i32* %buffer, i32 %t4
  %t6 = load i32, i32* %t0
  store i32 %t6, i32* %t5
  br label %for.inc2
for.inc2:
  %t7 = load i32, i32* %t0
  %t8 = add i32 %t7, 1
  store i32 %t8, i32* %t0
  br label %for.cond0
for.end3:
  ret i32 0
}
