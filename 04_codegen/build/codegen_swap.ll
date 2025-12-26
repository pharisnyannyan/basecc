; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @swap(i32* %left, i32* %right) {
entry:
  %t0 = alloca i32
  %t1 = load i32, i32* %left
  store i32 %t1, i32* %t0
  %t2 = load i32, i32* %right
  store i32 %t2, i32* %left
  %t3 = load i32, i32* %t0
  store i32 %t3, i32* %right
  ret i32 0
}
