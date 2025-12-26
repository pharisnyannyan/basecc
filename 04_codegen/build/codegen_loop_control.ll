; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @loop_control() {
entry:
  %t0 = alloca i32
  store i32 0, i32* %t0
  br label %while.cond0
while.cond0:
  %t1 = icmp ne i32 1, 0
  br i1 %t1, label %while.body1, label %while.end2
while.body1:
  %t2 = load i32, i32* %t0
  %t3 = add i32 %t2, 1
  store i32 %t3, i32* %t0
  br label %while.end2
while.end2:
  %t4 = alloca i32
  store i32 2, i32* %t4
  br label %for.cond3
for.cond3:
  %t5 = load i32, i32* %t4
  %t6 = icmp ne i32 %t5, 0
  br i1 %t6, label %for.body4, label %for.end6
for.body4:
  %t7 = load i32, i32* %t4
  %t8 = icmp ne i32 %t7, 0
  br i1 %t8, label %if.then7, label %if.end8
if.then7:
  br label %for.inc5
if.end8:
  %t9 = load i32, i32* %t0
  %t10 = add i32 %t9, 10
  store i32 %t10, i32* %t0
  br label %for.inc5
for.inc5:
  %t11 = load i32, i32* %t4
  %t12 = sub i32 %t11, 1
  store i32 %t12, i32* %t4
  br label %for.cond3
for.end6:
  %t13 = load i32, i32* %t0
  ret i32 %t13
}
