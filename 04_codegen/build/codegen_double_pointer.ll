; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @set_value(i32** %target, i32 %value) {
entry:
  %t0 = load i32*, i32** %target
  store i32 %value, i32* %t0
  %t1 = load i32*, i32** %target
  %t2 = load i32, i32* %t1
  ret i32 %t2
}
define i32 @increment_via_pointer(i32** %target) {
entry:
  %t0 = load i32*, i32** %target
  %t1 = load i32*, i32** %target
  %t2 = load i32, i32* %t1
  %t3 = add i32 %t2, 1
  store i32 %t3, i32* %t0
  %t4 = load i32*, i32** %target
  %t5 = load i32, i32* %t4
  ret i32 %t5
}
