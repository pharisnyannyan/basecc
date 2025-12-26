; ModuleID = 'basecc'
source_filename = "basecc"

@value = global i32 7
define i32* @main() {
entry:
  ret i32* @value
}
