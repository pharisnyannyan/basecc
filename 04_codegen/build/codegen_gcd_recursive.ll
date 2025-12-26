; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @gcd_recursive(i32 %a, i32 %b) {
entry:
  %t0 = icmp eq i32 %b, 0
  %t1 = zext i1 %t0 to i32
  %t2 = icmp ne i32 %t1, 0
  br i1 %t2, label %if.then0, label %if.end1
if.then0:
  ret i32 %a
if.end1:
  %t3 = srem i32 %a, %b
  %t4 = call i32 @gcd_recursive(i32 %b, i32 %t3)
  ret i32 %t4
}
