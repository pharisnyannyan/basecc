; ModuleID = 'basecc'
source_filename = "basecc"

@global_value = global i32 5
@static_value = internal global i32 0
@static_flag = internal global i8 1
@static_numbers = internal global [2 x i16] zeroinitializer
@static_ptr = internal global i32* @global_value
define internal i32 @add(i32 %left, i32 %right) {
entry:
  %t0 = add i32 %left, %right
  ret i32 %t0
}
@.static.main.0.local_total = internal global i32 3
@.static.main.1.local_letter = internal global i8 0
@.static.main.2.local_const = internal global i16 2
@.static.main.3.local_ptr = internal global i32* @global_value
@.static.main.4.local_array = internal global [2 x i32] zeroinitializer

define i32 @main() {
entry:
  %t5 = alloca i32
  %t6 = load i32, i32* @.static.main.0.local_total
  %t7 = load i16, i16* @.static.main.2.local_const
  %t8 = sext i16 %t7 to i32
  %t9 = call i32 @add(i32 %t6, i32 %t8)
  store i32 %t9, i32* %t5
  %t10 = load i32, i32* %t5
  store i32 %t10, i32* @.static.main.0.local_total
  %t11 = load i32, i32* @.static.main.0.local_total
  ret i32 %t11
}
