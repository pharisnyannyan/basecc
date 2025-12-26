; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @foo(i32 %a, i32 %b) {
entry:
  %t0 = add i32 %a, %b
  ret i32 %t0
}
define i32 @main() {
entry:
  %t0 = call i32 @foo(i32 3, i32 4)
  ret i32 %t0
}
