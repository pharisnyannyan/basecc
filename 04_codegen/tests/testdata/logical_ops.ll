; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @not_zero() {
entry:
  %t0 = icmp eq i32 0, 0
  %t1 = zext i1 %t0 to i32
  ret i32 %t1
}
define i32 @logical_and() {
entry:
  br label %logic.left0
logic.left0:
  %t0 = icmp ne i32 1, 0
  br i1 %t0, label %logic.rhs1, label %logic.end2
logic.rhs1:
  %t1 = icmp ne i32 0, 0
  br label %logic.end2
logic.end2:
  %t2 = phi i1 [0, %logic.left0], [%t1, %logic.rhs1]
  %t3 = zext i1 %t2 to i32
  ret i32 %t3
}
define i32 @logical_or() {
entry:
  br label %logic.left0
logic.left0:
  %t0 = icmp ne i32 0, 0
  br i1 %t0, label %logic.end2, label %logic.rhs1
logic.rhs1:
  %t1 = icmp ne i32 1, 0
  br label %logic.end2
logic.end2:
  %t2 = phi i1 [1, %logic.left0], [%t1, %logic.rhs1]
  %t3 = zext i1 %t2 to i32
  ret i32 %t3
}
define i32 @short_circuit_and_div0() {
entry:
  br label %logic.left0
logic.left0:
  %t0 = icmp ne i32 0, 0
  br i1 %t0, label %logic.rhs1, label %logic.end2
logic.rhs1:
  %t1 = sdiv i32 1, 0
  %t2 = icmp ne i32 %t1, 0
  br label %logic.end2
logic.end2:
  %t3 = phi i1 [0, %logic.left0], [%t2, %logic.rhs1]
  %t4 = zext i1 %t3 to i32
  ret i32 %t4
}
define i32 @short_circuit_or_div0() {
entry:
  br label %logic.left0
logic.left0:
  %t0 = icmp ne i32 1, 0
  br i1 %t0, label %logic.end2, label %logic.rhs1
logic.rhs1:
  %t1 = sdiv i32 1, 0
  %t2 = icmp ne i32 %t1, 0
  br label %logic.end2
logic.end2:
  %t3 = phi i1 [1, %logic.left0], [%t2, %logic.rhs1]
  %t4 = zext i1 %t3 to i32
  ret i32 %t4
}
