; ModuleID = 'basecc'
source_filename = "basecc"

%struct.Pair = type { i32, i8 }

@value = global %struct.Pair zeroinitializer
define i32 @struct_smoke() {
entry:
  ret i32 1
}
