; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @add() {
entry:
  %t0 = add i32 3, 4
  ret i32 %t0
}
define i32 @sub() {
entry:
  %t0 = sub i32 10, 2
  ret i32 %t0
}
define i32 @mul() {
entry:
  %t0 = mul i32 6, 7
  ret i32 %t0
}
define i32 @divide() {
entry:
  %t0 = sdiv i32 20, 4
  ret i32 %t0
}
define i32 @mod() {
entry:
  %t0 = srem i32 20, 6
  ret i32 %t0
}
define i32 @mixed() {
entry:
  %t0 = add i32 1, 2
  %t1 = mul i32 %t0, 3
  %t2 = sdiv i32 4, 2
  %t3 = srem i32 %t2, 3
  %t4 = sub i32 %t1, %t3
  ret i32 %t4
}
define i32 @unary() {
entry:
  %t0 = add i32 1, 2
  %t1 = sub i32 0, %t0
  %t2 = add i32 %t1, 3
  ret i32 %t2
}
define i32 @nested_parens() {
entry:
  %t0 = add i32 1, 2
  %t1 = sub i32 3, 4
  %t2 = mul i32 %t0, %t1
  %t3 = add i32 5, 6
  %t4 = sdiv i32 %t2, %t3
  ret i32 %t4
}
define i32 @triple_nested() {
entry:
  %t0 = add i32 1, 2
  %t1 = add i32 %t0, 3
  %t2 = sub i32 5, 6
  %t3 = add i32 4, %t2
  %t4 = mul i32 %t1, %t3
  ret i32 %t4
}
