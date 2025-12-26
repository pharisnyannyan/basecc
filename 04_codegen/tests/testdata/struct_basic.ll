; ModuleID = 'basecc'
source_filename = "basecc"

%struct.Pair = type { i32, i8 }

@value = global %struct.Pair zeroinitializer
define i32 @use_struct() {
entry:
  ret i32 1
}
