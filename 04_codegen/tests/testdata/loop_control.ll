; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @main() {
entry:
  br label %while.cond0
while.cond0:
  %t0 = icmp ne i32 1, 0
  br i1 %t0, label %while.body1, label %while.end2
while.body1:
  br label %while.end2
while.end2:
  br label %for.cond3
for.cond3:
  br label %for.body4
for.body4:
  br label %for.inc5
for.inc5:
  br label %for.cond3
for.end6:
  ret i32 0
}
