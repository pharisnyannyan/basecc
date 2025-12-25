; ModuleID = 'basecc'
source_filename = "basecc"

@value = global i32 7
@ptr = global i32* @value
define i32 @main() {
entry:
  %t0 = load i32*, i32** @ptr
  %t1 = load i32, i32* %t0
  ret i32 %t1
}
