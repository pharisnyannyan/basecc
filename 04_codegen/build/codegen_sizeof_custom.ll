; ModuleID = 'basecc'
source_filename = "basecc"

%struct.Custom = type { i8, i32, i16 }

define i32 @size_custom_type() {
entry:
  %t0 = getelementptr %struct.Custom, %struct.Custom* null, i32 1
  %t1 = ptrtoint %struct.Custom* %t0 to i32
  ret i32 %t1
}
define i32 @size_custom_value() {
entry:
  %t0 = alloca %struct.Custom
  %t1 = getelementptr %struct.Custom, %struct.Custom* null, i32 1
  %t2 = ptrtoint %struct.Custom* %t1 to i32
  ret i32 %t2
}
