; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @main() {
entry:
  br label %while.cond0
while.cond0:
  %t0 = icmp ne i32 0, 0
  br i1 %t0, label %while.body1, label %while.end2
while.body1:
  br label %while.cond0
while.end2:
  %t1 = icmp ne i32 1, 0
  br i1 %t1, label %if.then3, label %if.else4
if.then3:
  ret i32 2
if.else4:
  ret i32 3
}
