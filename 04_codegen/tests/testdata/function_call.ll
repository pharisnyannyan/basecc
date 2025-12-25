; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @foo() {
entry:
  ret i32 7
}
define i32 @main() {
entry:
  %t0 = call i32 @foo()
  ret i32 %t0
}
